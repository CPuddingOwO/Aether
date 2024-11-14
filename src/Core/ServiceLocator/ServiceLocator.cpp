#include <Aether/Core/ServiceLocator/ServiceLocator.hpp>

namespace Aether {

    std::unordered_map<std::type_index, std::shared_ptr<void>> ServiceLocator::services;
    std::mutex ServiceLocator::mutex_;

}