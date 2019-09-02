/*
 * Copyright 2017-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>

#include "ConfigurationNode.h"
#include <stdio.h>
#include <string.h>


//输出调试信息宏
#define LOG_RUN_DEBUG(format,...)  do{\
        printf("DEBUG:[%s:%d][%s]" format "\n\r", __FILE__,__LINE__ , __FUNCTION__,   ##__VA_ARGS__);\
    }while(0)




using namespace rapidjson;

std::mutex ConfigurationNode::m_mutex;
Document ConfigurationNode::m_document;
ConfigurationNode ConfigurationNode::m_root;


static std::string valueToString(const Value& value) {
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    if (value.Accept(writer)) {
        return stringBuffer.GetString();
    }
    LOG_RUN_DEBUG("valueToStringFailed  AcceptReturnedFalse");
    return "";
}



/**
 * Deep (possibly recursive) merge of two @c rapidjson values of type @c rapidjson::Type::kObject. The contents
 * of @c in are merged in to @c out. Values that occur in both are replaced by the values in @c in.
 *
 * @param path String describing the accumulated path to the sub-objects being merged.
 * @param[in,out] out The object that @c in will be merged in to.
 * @param in The object to merge in to @c out.
 * @param allocator Allocator of the @c rapidjson::Document containing @c out.
 */
static void mergeDocument(const std::string& path, Value& out, Value& in, Document::AllocatorType& allocator) {
    for (auto inIt = in.MemberBegin(); inIt != in.MemberEnd(); inIt++) {
        auto outIt = out.FindMember(inIt->name);
        if (outIt != out.MemberEnd() && inIt->value != outIt->value) {
            auto memberPath = path + "." + outIt->name.GetString();
            if (outIt->value.IsObject()) {
                LOG_RUN_DEBUG("memberPath=%s",memberPath.c_str());
                mergeDocument(memberPath, outIt->value, inIt->value, allocator);
            } else {
                LOG_RUN_DEBUG("memberPath=%s   old=%s  new=%s",memberPath.c_str(), valueToString(outIt->value).c_str(),valueToString(inIt->value).c_str());
                outIt->value = inIt->value;
            }
        } else {
            out.AddMember(inIt->name, inIt->value, allocator);
        }
    }
}


#if  0

//std::string  configFile = "/tmp/test.json";

//std::vector<std::shared_ptr<std::istream>> configJsonStreams;
auto configInFile = std::shared_ptr<std::ifstream>(new std::ifstream(configFile));
if (!configInFile->good()) {
    //  ACSDK_CRITICAL(LX("Failed to read config file").d("filename", configFile));
    //  alexaClientSDK::sampleApp::ConsolePrinter::simplePrint("Failed to read config file " + configFile);
    return false;
}
// configJsonStreams.push_back(configInFile);

#endif

bool ConfigurationNode::initialize(const std::vector<std::shared_ptr<std::istream>>& jsonStreams) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_root) {
        LOG_RUN_DEBUG("alreadyInitialized");
        return false;
    }
    m_document.SetObject();

    for (auto jsonStream : jsonStreams) {
        if (!jsonStream) {
            m_document.SetObject();
            return false;
        }
        IStreamWrapper wrapper(*jsonStream);
        Document overlay(&m_document.GetAllocator());
        overlay.ParseStream<kParseCommentsFlag>(wrapper);
        if (overlay.HasParseError()) {
            // LOG_RUN_DEBUG("initializeFailed   offset=%s    message=%s", overlay.GetErrorOffset().c_str() ,GetParseError_En(overlay.GetParseError()).c_str()  );
            LOG_RUN_DEBUG("initializeFailed");
            m_document.SetObject();

            return false;
        }

        mergeDocument("root", m_document, overlay, m_document.GetAllocator());
    }

    m_root = ConfigurationNode(&m_document);
    LOG_RUN_DEBUG("configuration=%s", valueToString(m_document).c_str());
    return true;
}

void ConfigurationNode::uninitialize() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_document.SetObject();
    m_root = ConfigurationNode();
}

ConfigurationNode ConfigurationNode::getRoot() {
    return m_root;
}

ConfigurationNode::ConfigurationNode() : m_object {nullptr} {
}

bool ConfigurationNode::getBool(const std::string& key, bool* out, bool defaultValue) const {
    return getValue(key, out, defaultValue, &Value::IsBool, &Value::GetBool);
}

bool ConfigurationNode::getInt(const std::string& key, int* out, int defaultValue) const {
    return getValue(key, out, defaultValue, &Value::IsInt, &Value::GetInt);
}

bool ConfigurationNode::getString(const std::string& key, std::string* out, std::string defaultValue) const {
    const char* temp;
    auto result = getString(key, &temp, defaultValue.c_str());
    if (out) {
        *out = temp;
    }
    return result;
}

bool ConfigurationNode::getString(const std::string& key, const char** out, const char* defaultValue) const {
    return getValue(key, out, defaultValue, &Value::IsString, &Value::GetString);
}

ConfigurationNode ConfigurationNode::operator[](const std::string& key) const {
    if (!*this) {
        return ConfigurationNode();
    }
    auto it = m_object->FindMember(key.c_str());
    if (m_object->MemberEnd() == it || !it->value.IsObject()) {
        return ConfigurationNode();
    }
    return ConfigurationNode(&it->value);
}

ConfigurationNode::operator bool() const {
    return m_object;
}

ConfigurationNode::ConfigurationNode(const rapidjson::Value* object) : m_object {object} {
}

std::string ConfigurationNode::serialize() const {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    if (!m_object->Accept(writer)) {
        LOG_RUN_DEBUG("writerRefusedObject");
        return "";
    }

    const char* bufferData = buffer.GetString();
    if (!bufferData) {
        LOG_RUN_DEBUG("nullptrBufferString");
        return "";
    }

    return std::string(bufferData);
}

