#include "XMLReader.h"
#include "XMLEntity.h"
#include <expat.h>
#include <queue>
#include <vector>

struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> source;
    XML_Parser parser;
    std::queue<SXMLEntity> queue;
    std::string chardata;
    bool dataend;

    static void handlestart(void *data, const char *name, const char ** attributes) {
        auto * impl = static_cast<SImplementation *>(data);

        if (!impl->chardata.empty()) {
            SXMLEntity ent; 
            ent.DType = SXMLEntity::EType::CharData;
            ent.DNameData = impl->chardata;
            impl->queue.push(ent);
            impl->chardata.clear();
        }

        SXMLEntity ent;
        ent.DType = SXMLEntity::EType::StartElement; 
        ent.DNameData = name;

        for (int i = 0; attributes[i]; i += 2){
            if (attributes[i + 1]) {
                ent.DAttributes.emplace_back(attributes[i], attributes[i + 1]);
            }
        }

        impl->queue.push(ent);
    }

    static void handleend(void * data, const char * name) {
        auto *impl = static_cast<SImplementation *>(data);

        if (!impl->chardata.empty()) {
            SXMLEntity ent; 
            ent.DType = SXMLEntity::EType::CharData;
            ent.DNameData = impl->chardata;
            impl->queue.push(ent);
            impl->chardata.clear();
        }

        SXMLEntity ent; 
        ent.DType = SXMLEntity::EType::EndElement;
        ent.DNameData = name;
        impl->queue.push(ent);
    }

    static void handlechar(void * data, const char * s, int length) {
        auto * impl = static_cast<SImplementation *>(data);
        if (s && length > 0) {
            impl->chardata.append(s, length);
        }
    }

    SImplementation(std::shared_ptr<CDataSource> src) : source(src), dataend(false) {
        parser = XML_ParserCreate(nullptr);
        XML_SetUserData(parser, this);
        XML_SetElementHandler(parser, handlestart, handleend);
        XML_SetCharacterDataHandler(parser, handlechar);
    }

    ~SImplementation() {
        XML_ParserFree(parser);
    }


    bool ReadEntity(SXMLEntity &entity, bool skipCharData = false) {
        std::vector<char> buffer(4096);

        if (!queue.empty()) {
            entity = queue.front();
            queue.pop();

            if (skipCharData && entity.DType == SXMLEntity::EType::CharData) {
                return ReadEntity(entity, skipCharData);
            }

            return true;
        }

        while (!dataend && queue.empty()) {
            size_t bytes = 0;

            while (bytes < buffer.size() && !source->End()) {
                char c;
                if (source->Get(c)) {
                    buffer[bytes++] = c;
                } else {
                    break;
                }
            }
            if (bytes == 0) {
                dataend = true; 
                return false;
            }

            if (XML_Parse(parser, buffer.data(), bytes, bytes == 0) == XML_STATUS_ERROR) {
                return false;
            }

            if (!queue.empty()) {
                entity = queue.front();
                queue.pop();

                if (skipCharData && entity.DType == SXMLEntity::EType::CharData) {
                    return ReadEntity(entity, skipCharData);
                }
                return true;
            }

        }
        
        return false;
    }

};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src) : DImplementation(std::make_unique<SImplementation>(std::move(src))) {

}

CXMLReader::~CXMLReader() = default;

bool CXMLReader::End() const {
    return DImplementation->dataend && DImplementation->queue.empty();
}

bool CXMLReader::ReadEntity(SXMLEntity& entity, bool skipCharData) {
    return DImplementation->ReadEntity(entity, skipCharData);
}
