#include "OpenStreetMap.h"
#include "XMLReader.h"
#include <unordered_map> // to store nodes/ways by id
#include <vector>
#include <string>
#include <memory> // to use smart pointers

struct COpenStreetMap::SImplementation {
    struct SNodeImpl : public CStreetMap::SNode { // implementation of SNode
        TNodeID NodeID; // initiate its variables
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

    std::vector<std::shared_ptr<SNodeImpl>> nodes; // initialize vector to store nodes
    
    struct SWayImpl : public CStreetMap::SWay { // implementation of SWay
        TWayID wayID; 
        std::vector<TNodeID> nodeids;
        std::unordered_map<std::string, std::string> attributes;

        TWayID ID() const noexcept override {
            return wayID;
        }

        std::size_t NodeCount() const noexcept override {
            return nodeids.size();
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

    void parse(std::shared_ptr<CXMLReader> src) { // parses the OpenStreetMap file
        SXMLEntity ent;
        std::shared_ptr<SNodeImpl> currNode = nullptr; // shared pointers to track current node/way
        std::shared_ptr<SWayImpl> currWay = nullptr;

        while (src->ReadEntity(ent)) {
            if (ent.DType == SXMLEntity::EType::StartElement) {
                if (ent.DNameData == "node") { // to process a node
                    currNode = std::make_shared<SNodeImpl>(); // create a new node
    
                    for (const auto & attribute : ent.DAttributes) {
                        if (attribute.first == "id") {
                            currNode->NodeID = std::stoull(attribute.second); // stoull converts string to unsigned long long
                        } else if (attribute.first == "lat") { // latitude converted to double and stored
                            currNode->NLocation.first = std::stod(attribute.second);
                        } else if (attribute.first == "lon") { // longitude also converted
                            currNode->NLocation.second = std::stod(attribute.second);
                        } else { // other attributes
                            currNode->attributes[attribute.first] = attribute.second;
                        }
                    }
    
                } else if (ent.DNameData == "way") { // to process a way
                    currWay = std::make_shared<SWayImpl>(); // create a new way
    
                    for (const auto & attribute : ent.DAttributes) {
                        if (attribute.first == "id") {
                            currWay->wayID = std::stoull(attribute.second); // store the way ID
                        } else {
                            currWay->attributes[attribute.first] = attribute.second;
                        }
                    }
    
                } else if (ent.DNameData == "nd" && currWay) { // process node reference in way
                    for (const auto & attribute : ent.DAttributes) {
                        if (attribute.first == "ref") {
                            currWay->nodeids.push_back(std::stoull(attribute.second)); // add node ID to the way's node list
                        }
                    }
                } else if (ent.DNameData == "tag") { // processing tag element for both node/way
                    std::string k; // initialize key and value 
                    std::string v;
    
                    for (const auto & attribute : ent.DAttributes) {
                        if (attribute.first == "k") {
                            k = attribute.second; // store key
                        }
                        if (attribute.first == "v") {
                            v = attribute.second; // store value
                        }
                    }
    
                    if (!k.empty()) {
                        if (currWay) {
                            currWay->attributes[k] = v; // Add the key, value pair to the way
                        }
                        if (currNode) {
                            currNode->attributes[k] = v; // Add the key, value pair to the node
                        }
                    }
                }
            } else if (ent.DType == SXMLEntity::EType::EndElement) {
                if (ent.DNameData == "node" && currNode) {
                    nodes.push_back(currNode); // add the node to the vector and map
                    currNode = nullptr;
                } else if (ent.DNameData == "way" && currWay) {
                    ways.push_back(currWay); // add the way to the vector and map
                    currWay = nullptr;
                }
            }
        }
    }
};

// constructor
COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src) : DImplementation(std::make_unique<SImplementation>()){
    DImplementation->parse(src);
}

// destructor
COpenStreetMap::~COpenStreetMap() = default;

// return the number of nodes
std::size_t COpenStreetMap::NodeCount() const noexcept {
    return DImplementation->nodes.size();
}

// return the number of ways
std::size_t COpenStreetMap::WayCount() const noexcept {
    return DImplementation->ways.size();
}

// get node by index
std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->nodes.size()) {
        return DImplementation->nodes[index];
    }
    return nullptr; // return nullptr if index out of bound
}

// get node by ID
std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept {
    for (auto & n : DImplementation->nodes) {
        if (n->ID() == id) {
            return n;
        }
    }
    return nullptr; // return nullptr if node not found
}

// get way by index
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->ways.size()) {
        return DImplementation->ways[index];
    }
    return nullptr; // return nullptr if index out of bound
}

// get way by ID
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept {
    for (auto & w : DImplementation->ways) {
        if (w->ID() == id) {
            return w;
        }
    }
    return nullptr; // return nullptr if way not found
}
