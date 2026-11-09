// SynTHBloK-Quad: registers this plugin's modules.
#include "plugin.hpp"

Plugin* pluginInstance;

extern Model* modelSynTHBloK;

void init(Plugin* p) {
    pluginInstance = p;
    p->addModel(modelSynTHBloK);
}
