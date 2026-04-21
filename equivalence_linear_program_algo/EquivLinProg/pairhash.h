//
// Created by Admin on 2026-03-06.
//

#ifndef EQUIVLINPROG_PAIRHASH_H
#define EQUIVLINPROG_PAIRHASH_H
// Customized hash struct for std::pair
struct PairHash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // A way to combine hashes, there are more robust methods ...
        return h1 ^ (h2 << 1);
    }
};

#endif //EQUIVLINPROG_PAIRHASH_H