#include "OpenStreetMap.h"
#include <unordered_map> // to store nodes/ways by id
#include <vector> 
#include <memory> // to use smart pointers

struct COpenStreetMap::SImplementation {
    struct SNodeImpl : public CStreetMap::SNode { // implementation of SNode
        TNodeID NodeID; // intitiate its variables
        TLocation NLocation;
        std::unordered_map<std::string, std::string> attributes;

        TNodeID ID() const noexcept override {
            return NodeID;
        }

        TLocation Location() const noexcept override {
            return NLocation;
        }
        
        std::size_t AttributeCount() const noexcept override {
            return attributes.size();
        }

        std::string GetAttributeKey(std::size_t i) const noexcept override {
            if (i >= attributes.size()) {
                return ""; // return "" if index goes out of bounds
            }
            auto iterate = attributes.begin();
            for (std::size_t j = 0; j < i; j++) {
                iterate++;
            }
            return iterate->first; // return key at given index
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
                return ""; // return "" if key out of bounds
            } else {
                return iterate->second; // return the attribute 
            }
        }
    };

    std::vector<std::shared_ptr<SNodeImpl>> nodes; // intitialize vector to store nodes
    std::unordered_map<TNodeID, std::shared_ptr<SNodeImpl>> nMap; // intitialize map to store ids and the node
   
    struct SWayImpl : public CStreetMap::SWay { // implementationn of SWay, this is all very similar to SNodeImpl
        TWayID wayID; 
        std::vector<TNodeID> nodeids;
        std::unordered_map<std::string, std::string> attributes;

        TWayID ID() const noexcept override {
            return wayID;
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

    std::vector<std::shared_ptr<SWayImpl>> ways; // initialize vector to store ways
    std::unordered_map<TWayID, std::shared_ptr<SWayImpl>> wMap; // initialize map to store id and the way

    void parse(std::shared_ptr<CXMLReader> src) { // this function is what parses the open street map file
        SXMLEntity ent;
        std::shared_ptr<SImplementation::SNodeImpl> currNode = nullptr; // shared pointers to track current node/way
        std::shared_ptr<SImplementation::SWayImpl> currWay = nullptr;

        while (src->ReadEntity(ent)) {
            if (ent.DType != SXMLEntity::EType::StartElement) { // only start elements processed
                break;
            }
            if (ent.DNameData == "node") { // if a node
                currNode = std::make_shared<SImplementation::SNodeImpl>(); // create a node implementation

                for (const auto & attribute : ent.DAttributes) { // id converted to TNodeID
                    if (attribute.first == "id") {
                        currNode->NodeID = std::stoull(attribute.second); // stoull converts string to unsigned long long
                    } else if (attribute.first == "lat") { // latitude converted to double and stored
                        currNode->NLocation.first = std::stod(attribute.second);
                    } else if (attribute.first == "lon") { // same for longitude
                        currNode->NLocation.second = std::stod(attribute.second);
                    } else { // for other attributes
                        currNode->attributes[attribute.first] = attribute.second;
                    }
                }
                nodes.push_back(currNode); // add currNode to nodes vector and nMap
                nMap[currNode->NodeID] = currNode;
            }

            if (ent.DNameData == "way") { // similar to above but for way, which has less attributes
                currWay = std::make_shared<SImplementation::SWayImpl>();

                for (const auto & attribute : ent.DAttributes) {
                    if (attribute.first == "id") {
                        currWay->wayID = std::stoull(attribute.second);
                    } else {
                        currWay->attributes[attribute.first] = attribute.second;
                    }
                }
                ways.push_back(currWay);
                wMap[currWay->wayID] = currWay;
            }

            if (ent.DNameData == "nd") { 
                for (const auto & attribute : ent.DAttributes) {
                    if (attribute.first == "ref") { // contains node id in way
                        currWay->nodeids.push_back(std::stoull(attribute.second)); // add to nodeids
                    }
                }
            }

            if (ent.DNameData == "tag") { // has key anv value pair
                std::string k; // initialize key and value 
                std::string v;

                for (const auto & attribute : ent.DAttributes) {
                    if (attribute.first == "k") {
                        k = attribute.second; // store key in k
                    }
                    if (attribute.first == "v") {
                        v = attribute.second; // store valye in v
                    }
                }

                if (k.empty() == false) {
                    if (currWay) {
                        currWay->attributes[k] = v; // add key, valye to currWay
                    } 
                    if (currNode) {
                        currNode->attributes[k] = v; // add key, value to currNode
                    }
                }
            }    
        }
    };
};

// constructors 

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