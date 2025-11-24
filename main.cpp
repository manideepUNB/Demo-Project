#include "task_scheduler.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "sphere.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <atomic>

// Image settings
const auto aspect_ratio = 16.0 / 9.0;
const int image_width = 400;
const int image_height = static_cast<int>(image_width / aspect_ratio);

// Scene
sphere world(point3(0, 0, -1), 0.5);

// Ray Color Function
color ray_color(const ray& r) {
    double t;
    vec3 N;
    if (world.hit(r, 0, 1000, t, N)) {
        return 0.5 * (N + color(1,1,1));
    }
    vec3 unit_direction = unit_vector(r.direction());
    t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

// Global buffer to store image data
std::vector<color> image_buffer(image_width * image_height);
std::atomic<int> scanlines_remaining;

// Coroutine to render a single scanline
Task render_scanline(ThreadPool& pool, int j) {
    // Switch to worker thread
    co_await schedule(pool);

    // Camera settings
    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);

    for (int i = 0; i < image_width; ++i) {
        auto u = double(i) / (image_width-1);
        auto v = double(j) / (image_height-1);
        ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin);
        color pixel_color = ray_color(r);
        
        // Store in buffer (flip Y for PPM)
        image_buffer[(image_height - 1 - j) * image_width + i] = pixel_color;
    }

    scanlines_remaining.fetch_sub(1, std::memory_order_relaxed);
}

int main() {
    std::cout << "Rendering " << image_width << "x" << image_height << " image...\n";
    
    ThreadPool pool(std::thread::hardware_concurrency());
    scanlines_remaining = image_height;

    // Schedule all scanlines
    for (int j = image_height - 1; j >= 0; --j) {
        render_scanline(pool, j);
    }

    // Wait for all scanlines to complete
    while (scanlines_remaining > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Write to PPM file
    std::ofstream ofs("image.ppm");
    ofs << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (const auto& pixel : image_buffer) {
        ofs << static_cast<int>(255.999 * pixel.x()) << ' '
            << static_cast<int>(255.999 * pixel.y()) << ' '
            << static_cast<int>(255.999 * pixel.z()) << '\n';
    }

    std::cout << "Done! Saved to image.ppm\n";
    return 0;
}
