#pragma once

#include "bk_types.hpp"
#include "yaml-cpp-yaml-cpp-0.6.0/include/yaml-cpp/yaml.h"

void bk_dialog2yaml(const bk_dialog_file& dialog_file, YAML::Emitter& emitter);

bk_dialog_file bk_yaml2dialog(YAML::Node const& node);
