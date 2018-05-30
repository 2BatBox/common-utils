#ifndef BINIO_OPT_VALUE_H
#define BINIO_OPT_VALUE_H

namespace binio {

/**
 * Class binio::OptValue represents an option value.
 * No memory management is provided.
 */
template <typename T>
struct OptValue {
protected:
    T opt_value;
    bool has_opt_value;

public:

    OptValue() noexcept : opt_value(), has_opt_value(false) { };

    OptValue(const T& v) noexcept : opt_value(v), has_opt_value(true) { };

    OptValue& operator=(const T& v) noexcept
    {
        opt_value = v;
        has_opt_value = true;
        return *this;
    }

    OptValue& operator=(const OptValue& opt) noexcept
    {
        if (this != &opt) {
            opt_value = opt.opt_value;
            has_opt_value = opt.has_opt_value;
        }
        return *this;
    }

    inline bool has_value() const noexcept
    {
        return has_opt_value;
    }

    inline const T& value() const noexcept
    {
        return opt_value;
    }

    inline T& value() noexcept
    {
        return opt_value;
    }

    inline operator const T& () const noexcept
    {
        return opt_value;
    }

    inline operator T& () noexcept
    {
        return opt_value;
    }

    inline bool operator==(const OptValue& lv) const noexcept
    {
        return(has_opt_value && lv.has_opt_value && opt_value == lv.opt_value);
    }

    inline bool operator!=(const OptValue& lv) const noexcept
    {
        return not operator==(lv);
    }

};

}; // namespace binio

#endif /* BINIO_OPT_VALUE_H */

