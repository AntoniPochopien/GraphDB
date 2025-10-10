#include "property.hpp"
#include <fstream>
#include <iostream>


using namespace std;

namespace graphdb
{
    size_t PropertyValue::estimateSize() const
    {
        return visit([](auto &&arg) -> size_t
                          {
        using T = decay_t<decltype(arg)>;
        if constexpr (is_same_v<T,int> || is_same_v<T,double> || is_same_v<T,bool>) {
            return 1 + sizeof(arg);
        } else if constexpr (is_same_v<T,string>) {
            return 1 + sizeof(size_t) + arg.size();
        } else if constexpr (is_same_v<T,shared_ptr<PropertyMap>>) {
            size_t total = 1 + sizeof(size_t);
            for (const auto& [k,v] : *arg) {
                total += sizeof(size_t) + k.size();
                total += v.estimateSize(); 
            }
            return total;
        } else {
            return 0;
        } }, value);
    }

    void PropertyValue::serialize(ostream &out) const
    {
        visit([&out](auto &&arg)
                   {
            using T = decay_t<decltype(arg)>;
            if constexpr (is_same_v<T,int>) {
                char type = 0; out.write(&type,1); out.write(reinterpret_cast<const char*>(&arg), sizeof(arg));
            } else if constexpr (is_same_v<T,double>) {
                char type = 1; out.write(&type,1); out.write(reinterpret_cast<const char*>(&arg), sizeof(arg));
            } else if constexpr (is_same_v<T,bool>) {
                char type = 2; out.write(&type,1); out.write(reinterpret_cast<const char*>(&arg), sizeof(arg));
            } else if constexpr (is_same_v<T,string>) {
                char type = 3; out.write(&type,1);
                size_t len = arg.size();
                out.write(reinterpret_cast<const char*>(&len), sizeof(len));
                out.write(arg.c_str(), len);
            } else if constexpr (is_same_v<T,shared_ptr<PropertyMap>>) {
                char type = 4; out.write(&type,1);
                size_t count = arg->size();
                out.write(reinterpret_cast<const char*>(&count), sizeof(count));
                for (const auto& [k,v] : *arg) {
                    size_t klen = k.size();
                    out.write(reinterpret_cast<const char*>(&klen), sizeof(klen));
                    out.write(k.c_str(), klen);
                    v.serialize(out);
                }
            } }, value);
    }

    PropertyValue PropertyValue::deserialize(ifstream &in)
    {
        char type;
        in.read(&type, 1);

        switch (type)
        {
        case 0:
        {
            int val;
            in.read(reinterpret_cast<char *>(&val), sizeof(val));
            return PropertyValue(val);
        }
        case 1:
        {
            double val;
            in.read(reinterpret_cast<char *>(&val), sizeof(val));
            return PropertyValue(val);
        }
        case 2:
        {
            bool val;
            in.read(reinterpret_cast<char *>(&val), sizeof(val));
            return PropertyValue(val);
        }
        case 3:
        {
            size_t len;
            in.read(reinterpret_cast<char *>(&len), sizeof(len));
            string s(len, '\0');
            in.read(&s[0], len);
            return PropertyValue(s);
        }
        case 4:
        {
            size_t count;
            in.read(reinterpret_cast<char *>(&count), sizeof(count));
            PropertyMap map;
            for (size_t i = 0; i < count; ++i)
            {
                size_t klen;
                in.read(reinterpret_cast<char *>(&klen), sizeof(klen));
                string k(klen, '\0');
                in.read(&k[0], klen);
                PropertyValue v = PropertyValue::deserialize(in);
                map[k] = v;
            }
            return PropertyValue(map);
        }
        default:
            throw runtime_error("Unknown PropertyValue type");
        }
    }
}