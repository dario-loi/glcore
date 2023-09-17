/**
 * @file cubemap.hpp
 * @author Dario Loi
 * @brief Cube map texture wrapper.
 * @date 2023-08-25
 *
 * @copyright MIT License
 *
 */

#pragma once

#include "gl_functions.hpp"
#include "texture.hpp"
#include "utility.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace glcore {

class cubemap {

public:
    cubemap(std::span<std::byte*, 6> data, resolution res,
        texture_color color = { GL_RGBA, GL_RGBA, GL_FLOAT },
        texture_filter filter = { GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE },
        bool generate_mipmaps = false);
    ~cubemap() noexcept
    {
        if (m_id != 0) {
            glDeleteTextures(1, &m_id);
        }
    }

    // delete copy and copy assignment operators

    cubemap(const cubemap&) = delete;
    cubemap& operator=(const cubemap&) = delete;

    /**
     * @brief Construct a new cubemap object
     *
     * @param other the other cubemap object to move from.
     */
    cubemap(cubemap&& other) noexcept
        : m_id(other.m_id)
        , m_res(other.m_res)
        , m_color(other.m_color)
        , m_filter(other.m_filter)
    {
        other.m_id = 0;
    }

    /**
     * @brief Move assignment operator.
     *
     * @param other the other cubemap object to move from.
     * @return cubemap& a reference to this object.
     */
    cubemap& operator=(cubemap&& other) noexcept
    {
        if (this != &other) {
            m_id = other.m_id;
            m_color = other.m_color;
            m_res = other.m_res;
            m_filter = other.m_filter;
            other.m_id = 0;
        }
        return *this;
    }

    /**
     * @brief Bind the cubemap texture.
     *
     * @param slot the texture slot to bind the texture to.
     */
    void bind() const;

    /**
     * @brief Unbind the cubemap texture.
     *
     */
    static void unbind();

    /**
     * @brief Get the texture id.
     *
     * @return uint32_t the texture id.
     */
    [[nodiscard]] constexpr uint32_t id() const noexcept { return m_id; }

    /**
     * @brief Get the texture color.
     *
     * @return texture_color the texture color.
     */
    [[nodiscard]] constexpr texture_color color() const noexcept { return m_color; }

    /**
     * @brief Get the resolution of the cubemap.
     *
     * @return resolution the resolution of the cubemap.
     */
    [[nodiscard]] constexpr resolution res() const noexcept { return m_res; }

    /**
     * @brief Set the unit object to bind the texture to.
     *
     * @param unit_offset an offset that specifies the unit on which to bind the texture.
     */
    void set_unit(std::uint32_t unit_offset) const;

private:
    uint32_t m_id {};
    resolution m_res {};
    texture_color m_color {};
    texture_filter m_filter {};
};

cubemap::cubemap(std::span<std::byte*, 6> data, resolution res, texture_color color, texture_filter filter, bool generate_mipmaps)
    : m_res(res)
    , m_color(color)
    , m_filter(filter)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, filter.clamping);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, filter.clamping);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, filter.clamping);

    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, generate_mipmaps ? to_mipmap(filter.min_filter) : filter.min_filter);
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, filter.mag_filter);

    int i = 0;
    for (auto const& face : data) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i++, 0, m_color.internal_format, m_res.width, m_res.height, 0, m_color.format, m_color.datatype, face);
    }

    if (generate_mipmaps) {
        glGenerateTextureMipmap(m_id);
    }
};

void cubemap::bind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}

void cubemap::unbind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void cubemap::set_unit(std::uint32_t unit_offset) const
{
    glActiveTexture(GL_TEXTURE0 + unit_offset);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}

} // namespace glcore