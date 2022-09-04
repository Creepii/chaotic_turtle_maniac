#pragma once

namespace client {
    class GameObject {
        private:
            int uuid;
        public:
            GameObject() = default;
            virtual ~GameObject() = default;
    };
}