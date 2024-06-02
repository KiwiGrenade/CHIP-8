#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <memory>
#include <array>

struct Component {
    virtual inline void clear() = 0;
};
    /* CHIP-8 has 4KB memory (4096 bytes), from location 0x000 (0) to 0xFFF (4095):
     * + 0x000 (0) to 0x1FF (511) - CHIP-8 interpreter
     * + 0x200 (512) to 0xFFF (4095) - program memory (ETI 660 programs start at 0x600 (1536))*/
struct Memory : Component {
    static constexpr unsigned short size = 4096;
    typedef std::array<unsigned char, size> memory_type;
    std::unique_ptr<memory_type> memory;

    Memory(){
        memory = std::make_unique<memory_type>();
        std::fill(memory->begin(), memory->end(), 0);
    }

    inline void clear() { std::fill(memory->begin(), memory->end(), 0); }

    unsigned char& operator[](const std::size_t idx) { return (*memory)[idx]; }
    const unsigned char& operator[](const std::size_t idx) const { return (*memory)[idx]; }

    ~Memory() = default;
};

struct Screen : Component {
    static constexpr unsigned short size = 64 * 32;
    typedef std::array<bool, size> screen_type;
    std::unique_ptr<screen_type> screen;
    Screen(){
        screen = std::make_unique<screen_type>();
    }

    inline void clear() {
        std::fill(screen->begin(), screen->end(), false);
    }

    ~Screen() = default;
};

#endif //COMPONENTS_HPP
