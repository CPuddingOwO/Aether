#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <mutex>

namespace Aether {
    class ServiceLocator {
    public:
        // Shared_ptr: std::shared_ptr -> shared_ptr
        template<typename T>
        static void Provide(const std::shared_ptr<T>& service) {
            std::lock_guard<std::mutex> lock(mutex_);
            services[typeid(T)] = service;
        }
        // Shared_ptr: Raw pointer -> shared_ptr
        template<typename T>
        static void Provide(T *service) {
            std::lock_guard<std::mutex> lock(mutex_);
            services[typeid(T)] = std::shared_ptr<T>(service);
        }
        // Unique_ptr: std::unique_ptr -> Unique_ptr
        // TODO: Support Unique_ptr

        template<typename T>
        static std::shared_ptr<T> Get() {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = services.find(typeid(T));
            if (it != services.end()) {
                return std::static_pointer_cast<T>(it->second);
            }
            throw std::runtime_error("Service not found");
        }

        // TODO: Support Unique_ptr

        static void Destroy() {
            std::lock_guard<std::mutex> lock(mutex_);
            services.clear();
        }

    private:
        static std::unordered_map<std::type_index, std::shared_ptr<void>> services;
        static std::mutex mutex_;
    };
}