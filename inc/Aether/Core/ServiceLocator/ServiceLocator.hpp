#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <mutex>

namespace ae {
    class ServiceLocator {
    public:
        template<typename T>
        static void Provide(const std::shared_ptr<T>& service) {
            std::lock_guard<std::mutex> lock(mutex_);
            services[typeid(T)] = service;
        }

        template<typename T>
        static void Provide(T *service) {
            std::lock_guard<std::mutex> lock(mutex_);
            services[typeid(T)] = std::shared_ptr<T>(service);
        }

        template<typename T>
        static std::shared_ptr<T> Get() {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = services.find(typeid(T));
            if (it != services.end()) {
                return std::static_pointer_cast<T>(it->second);
            }
            return nullptr; // TODO: throw exception
        }

    private:
        static std::unordered_map<std::type_index, std::shared_ptr<void>> services;
        static std::mutex mutex_;
    };

    std::unordered_map<std::type_index, std::shared_ptr<void>> ServiceLocator::services;
    std::mutex ServiceLocator::mutex_;
}