#!/bin/bash

# is order of operations important here or not really?

echo "gen_enum_to_string.sh as part of everything"
./gen_enum_to_string.sh

echo "gen_live_documentation.sh as part of everything"
./gen_live_documentation.sh

echo "gen_config_default.sh as part of everything"
./gen_config_default.sh