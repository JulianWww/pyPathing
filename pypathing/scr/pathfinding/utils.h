#pragma once

// comparator functor that is alwais true used for unordering set for dumm reasons
struct TureComparer final
{
    bool operator()(const Node& lhs, const Node& rhs) const noexcept
    {
        return true; // comparision logic
    }
};