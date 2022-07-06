tsl_h="https://raw.github.com/Tessil/hopscotch-map/master/include/tsl/"
tsl_r="https://raw.github.com/Tessil/robin-map/master/include/tsl/"
smap="https://raw.github.com/greg7mdp/sparsepp/master/sparsepp/"
pmap="https://raw.github.com/greg7mdp/parallel-hashmap/"
martinus_r="https://raw.github.com/martinus/robin-hood-hashing/master/src/include/"
martinus_d="https://raw.github.com/martinus/unordered_dense/master/include/ankerl/"
skarupke="https://raw.github.com/skarupke/flat_hash_map/master/"

mkdir -p ankerl skarupke sparsepp tsl

wget $martinus_r"robin_hood.h" -O "ankerl/robin_hood.h"
wget $martinus_d"unordered_dense.h" -O "ankerl/unordered_dense.h"

#wget $skarupke"bytell_hash_map.hpp" -O "skarupke/bytell_hash_map.hpp"
wget $skarupke"flat_hash_map.hpp" -O "skarupke/flat_hash_map.hpp"

#wget $smap"spp.h" -O "sparsepp/spp.h"
#wget $smap"spp_config.h" -O "sparsepp/spp_config.h"
#wget $smap"spp_dlalloc.h" -O "sparsepp/spp_dlalloc.h"
#wget $smap"spp_memory.h" -O "sparsepp/spp_memory.h"
#wget $smap"spp_smartptr.h" -O "sparsepp/spp_smartptr.h"
#wget $smap"spp_stdint.h" -O "sparsepp/spp_stdint.h"
#wget $smap"spp_timer.h" -O "sparsepp/spp_timer.h"
#wget $smap"spp_traits.h" -O "sparsepp/spp_traits.h"
#wget $smap"spp_utils.h" -O "sparsepp/spp_utils.h"

#wget $tsl_h"hopscotch_growth_policy.h" -O "tsl/hopscotch_growth_policy.h"
#wget $tsl_h"hopscotch_hash.h"          -O "tsl/hopscotch_hash.h"
#wget $tsl_h"hopscotch_map.h"           -O "tsl/hopscotch_map.h"
wget $tsl_r"robin_growth_policy.h" -O "tsl/robin_growth_policy.h"
wget $tsl_r"robin_hash.h"          -O "tsl/robin_hash.h"
wget $tsl_r"robin_map.h"           -O "tsl/robin_map.h"
