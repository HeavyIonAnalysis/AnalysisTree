#ifndef ANALYSISTREE_INFRA_BRANCHHASHHELPER_HPP_
#define ANALYSISTREE_INFRA_BRANCHHASHHELPER_HPP_

namespace Impl {

inline void hash_combine(std::size_t& /*seed*/) {}

template<typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  hash_combine(seed, rest...);
}

inline std::size_t BranchConfigHasher(const AnalysisTree::BranchConfig& config) {
  using Type = AnalysisTree::Types;

  std::size_t hash = 0;
  hash_combine(hash, static_cast<AnalysisTree::ShortInt_t>(config.GetType()));

  auto hash_fields = [&hash](const std::map<std::string, AnalysisTree::ConfigElement>& fields_map, Type field_type) {
    for (auto& field : fields_map) {
      hash_combine(hash, field.first, field.second.id_, static_cast<AnalysisTree::ShortInt_t>(field_type));
    }
  };

  hash_fields(config.GetMap<float>(), Type::kFloat);
  hash_fields(config.GetMap<int>(), Type::kInteger);
  hash_fields(config.GetMap<bool>(), Type::kBool);
  return hash;
}

}// namespace Impl

#endif//ANALYSISTREE_INFRA_BRANCHHASHHELPER_HPP_
