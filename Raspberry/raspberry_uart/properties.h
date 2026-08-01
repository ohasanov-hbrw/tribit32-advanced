#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <map>
#include <string>

class Properties {
public:
	Properties();
	Properties(std::string path);

    void load(const std::string filename);
    void save(const std::string filename) const;
	void flush() const;
    
    std::string get(const std::string key) const;
    std::string get(const std::string key, const std::string defaultValue);
    void set(const std::string key, const std::string value);
    void remove(const std::string key);

    int getInt(const std::string key, int defaultValue = 0);
    void setInt(const std::string key, int value);

    long getLong(const std::string key, long defaultValue = 0L);
    void setLong(const std::string key, long value);

    float getFloat(const std::string key, float defaultValue = 0.0f);
    void setFloat(const std::string key, float value);

    bool getBool(const std::string key, bool defaultValue = false);
    void setBool(const std::string key, bool value);
	
	void addWhenMissing(bool s);
private:
	bool awm = false;
	std::string path;
    std::map<std::string, std::string> properties;
};

#endif // PROPERTIES_H
