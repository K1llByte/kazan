#pragma once

namespace kzn {

class Panel {
public:
    virtual ~Panel() = default;

    virtual void update(float delta_time) = 0;

    void set_enabled(bool enable) { m_enabled = enable; }

protected:
    Panel() = default;

protected:
    bool m_enabled = false;
};

} // namespace kzn