tsl_h="https://raw.github.com/Tessil/hopscotch-map/master/include/tsl/"
tsl_r="https://raw.github.com/Tessil/robin-map/master/include/tsl/"
tsl_s="https://raw.github.com/Tessil/sparse-map/master/include/tsl/"
greg="https://raw.github.com/greg7mdp/sparsepp/master/sparsepp/"
martinus="https://raw.github.com/martinus/robin-hood-hashing/master/src/include/"
skarupke="https://raw.github.com/skarupke/flat_hash_map/master/"

mkdir -p tsl sparsepp skarupke

wget $martinus"robin_hood.h" -O "robin_hood.h"

wget $skarupke"bytell_hash_map.hpp" -O "skarupke/bytell_hash_map.hpp"
wget $skarupke"flat_hash_map.hpp" -O "skarupke/flat_hash_map.hpp"

wget $greg"spp.h" -O "sparsepp/spp.h"
wget $greg"spp_config.h" -O "sparsepp/spp_config.h"
wget $greg"spp_dlalloc.h" -O "sparsepp/spp_dlalloc.h"
wget $greg"spp_memory.h" -O "sparsepp/spp_memory.h"
wget $greg"spp_smartptr.h" -O "sparsepp/spp_smartptr.h"
wget $greg"spp_stdint.h" -O "sparsepp/spp_stdint.h"
wget $greg"spp_timer.h" -O "sparsepp/spp_timer.h"
wget $greg"spp_traits.h" -O "sparsepp/spp_traits.h"
wget $greg"spp_utils.h" -O "sparsepp/spp_utils.h"

wget $tsl_h"hopscotch_growth_policy.h" -O "tsl/hopscotch_growth_policy.h"
wget $tsl_h"hopscotch_hash.h"          -O "tsl/hopscotch_hash.h"
wget $tsl_h"hopscotch_map.h"           -O "tsl/hopscotch_map.h"

wget $tsl_r"robin_growth_policy.h" -O "tsl/robin_growth_policy.h"
wget $tsl_r"robin_hash.h"          -O "tsl/robin_hash.h"
wget $tsl_r"robin_map.h"           -O "tsl/robin_map.h"

#wget $tsl_s"sparse_growth_policy.h" -O "tsl/sparse_growth_policy.h"
#wget $tsl_s"sparse_hash.h"          -O "tsl/sparse_hash.h"
#wget $tsl_s"sparse_map.h"           -O "tsl/sparse_map.h"
