#include "OpenStreetMap.h"
#include <unordered_map>
#include <vector>
#include <memory>

struct COpenStreetMap::SImplementation {
    struct SNodeImpl : public CStreetMap::SNode {
        TNodeID ID;
        TLocation Location;
        std::unordered_map<std::string, std::string> attributes;

        SNodeImpl(TNodeID id, TLocation location) :ID(id), Location(location) {
        }

        TNodeID ID() const noexcept override {
            return ID;
        }

        TLocation Location() const noexcept override {
            return Location;
        }
        
        std::size_t AttributeCount() const noexcept override {
            return attributes.size();
        }

        std::string GetAttributeKey(std::size_t i) const noexcept override {
            if (i >= attributes.size()) {
                return "";
            }
            auto iterate = attributes.begin();
            for (std::size_t j = 0; j < i; j++) {
                iterate++;
            }
            return iterate->first;
        }

        bool HasAttribute(const std::string & key) const noexcept override {
            if (attributes.find(key) != attributes.end()) {
                return true;
            } else {
                return false;
            }
        }

        std::string GetAttribute(const std::string & key) const noexcept override {
            auto iterate = attributes.find(key);
            if (iterate == attributes.end()) {
                return "";
            } else {
                return iterate->second;
            }
        }
    };

    std::vector<std::shared_ptr<SNodeImpl>> nodes;
    std::unordered_map<TNodeID, std::shared_ptr<SNodeImpl>> nMap;
   
    struct SWayImpl : public CStreetMap::SWay {
        TWayID ID; 
        std::vector<TNodeID> nodeids;
        std::unordered_map<std::string, std::string> attributes;

        SWayImpl(TWayID id) : ID(id) {
        }

        TWayID ID() const noexcept override {
            return ID;
        }

        std::size_t NodeCount() const noexcept override {
            return attributes.size();
        }

        std::string GetAttributeKey(std::size_t i) const noexcept override {
            if (i >= attributes.size()) {
                return "";
            }
            auto iterate = attributes.begin();
            for (std::size_t j = 0; j < i; j++) {
                iterate++;
            }
            return iterate->first;
        }

        bool HasAttribute(const std::string & key) const noexcept override {
            if (attributes.find(key) != attributes.end()) {
                return true;
            } else {
                return false;
            }
        }

        std::string GetAttribute(const std::string & key) const noexcept override {
            auto iterate = attributes.find(key);
            if (iterate == attributes.end()) {
                return "";
            } else {
                return iterate->second;
            }
        }
    };

    std::vector<std::shared_ptr<SWayImpl>> ways;
    std::unordered_map<TWayID, std::shared_ptr<SWayImpl>> wMap;

    void parse(std::shared_ptr<CXMLReader> src) {
        // do tomorrow
    }
};

COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src) : DImplementation(std::make_unique<SImplementation>()) {
    DImplementation->parse(src);
}

COpenStreetMap::~COpenStreetMap() = default;

std::size_t COpenStreetMap::NodeCount() const noexcept {
    return DImplementation->nodes.size();
}

std::size_t COpenStreetMap::WayCount() const noexcept {
    return DImplementation->ways.size();
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->nodes.size()) {
        return DImplementation->nodes[index];
    } else {
        return nullptr;
    }
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept {
    auto iterate = DImplementation->nMap.find(id);
    if (iterate != DImplementation->nMap.end()) {
        return iterate->second;
    } else {
        return nullptr;
    }
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->ways.size()) {
        return DImplementation->ways[index];
    } else {
        return nullptr;
    }
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept {
    auto iterate = DImplementation->wMap.find(id);
    if (iterate != DImplementation->wMap.end()) {
        return iterate->second;
    } else {
        return nullptr;
    }
}