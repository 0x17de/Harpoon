#ifndef INI_H
#define INI_H

#include <map>
#include <string>


/// Class for reading and modifying ini files.
/// Comments inside the ini file will be discarded.
/// The order inside the ini file is not preserved.
class Ini {
public:
    /// Map structure for all available entries
    using Entries = std::map<std::string, std::string>;
    /// Map structure for all available categories
    using Categories = std::map<std::string, Entries>;
private:
    /// Map for all available categories 
    Categories categories;

    /// Path to the configuration file
    std::string filename;
    /// Was the file modified?
    bool modified;
    /// Was the file newly created?
    bool newFile;
public:
    /// Uses the specified file for further operations
    explicit Ini(const std::string& filename);
    /// Destructor
    ~Ini();

    /// load the configuration file
    void load();

    /// writes the configuration file if anything changed.
    /// This operation can be forced.
    ///
    /// \param bForce Write the file even when no changes were made
    void write(bool bForce = false);

    /// Returns if the configuration file was newly created
    bool isNew() const;

    /// Begin iterator for all categories
    Categories::iterator begin();
    /// End iterator for all categories
    Categories::iterator end();
    /// Returns some entries pointer for a category
    Entries* getEntry(const std::string& category);
    /// Returns if a category exists
    bool hasCategory(const std::string& category);
    /// Gets or creates a category inside the ini file
    Entries& expectCategory(const std::string& category);
    /// Removes a category and all associated data from the ini file
    void deleteCategory(const std::string& category);
    /// Fetches some entry and returns if the fetched operation was successful (slow method)
    bool getEntry(const std::string& category, const std::string& entry, std::string& data);
    /// Fetches some entry and returns if the fetched operation was successful (preferred method)
    bool getEntry(Entries& entries, const std::string& entry, std::string& data);
    /// Sets some entry in the ini file (slow method)
    void setEntry(const std::string& category, const std::string& entry, const std::string& data);
    /// Sets some entry in the ini file (preferred method)
    void setEntry(Entries& entries, const std::string& entry, const std::string& data);
};

#endif
