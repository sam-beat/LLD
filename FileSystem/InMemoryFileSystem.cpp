#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include<shared_mutex>
using namespace std;

// ============================================
// 2. COMPOSITE DESIGN PATTERN: BASE ENTITY
// ============================================

class Entity{
protected:
    string name;
public:
    Entity(string name) : name(name) {}
    virtual ~Entity() = default;
    string getName() const { return name; }
    void setName(const string &newName) { name = newName; }

    /* Composite methods declared at the base level to avoid dynamic_cast */
    virtual bool isFolder() const = 0;

    virtual void addChild(shared_ptr<Entity> child) {
        throw runtime_error("Not supported. ");
    }

    virtual void removeChild(shared_ptr<Entity> child) {
        throw runtime_error("Not Supported. ");
    }

    virtual vector<shared_ptr<Entity>> getChildren() const { return {}; }

};

// =================================
// 3. COMPOSITE COMPONENT: FOLDER
// =================================
class Folder: public Entity {
    vector<shared_ptr<Entity>> children;
    public:
    Folder(string name) : Entity(name) {}

    bool isFolder() const override { return true; }

    void addChild(shared_ptr<Entity> child) override {
         children.push_back(child);
    }

    void removeChild(shared_ptr<Entity> child) override {
         children.erase(remove(children.begin(), children.end(), child));
    }

    vector<shared_ptr<Entity>> getChildren() const override {
        return children;
    }
    
};

// ===========================
// 3. LEAF COMPONENT: FILE
// ===========================
class File: public Entity {
    int size;
    public:
    File(string name, int size) : Entity(name), size(size) {}
    bool isFolder() const override{ return false; } 
};


// ================================
// 4. THE IN-MEMORY FILE SYSTEM
// ================================
class InMemoryFileSystem {
    shared_ptr<Folder> root;
    unordered_map<string, shared_ptr<Entity>> pathMap;

    // The central Read-write Mutex protecting the entire state
    mutable shared_mutex fs_mutex;

    // split path helper function to separate the parent path and entity.
    // This will return a pair of string, out of which the first one is the parent and second one is the entity.
    pair<string, string> splitPath(const string& path) {
        if(path == "/" or path.empty()) return {"/", ""};
        
        size_t pos = path.find_last_of('/');
        string parentPath = path.substr(0, pos);
        string entityName = path.substr(pos + 1);

        if(parentPath.empty()) parentPath = "/";

        return {parentPath, entityName};
    }

    // Note: internal helper functions assume the lock is ALREADY acquires by the caller
    //After a rename you have to recursviely change the pathmap keys of all the entities which are associated with this path
    void updatePathMapRecursive(const string& oldPath, const string& newPath, const shared_ptr<Entity>& entity) {
        if(entity->isFolder()) {
            for(const auto& child: entity->getChildren()){
                string oldChildPath = oldPath + (oldPath == "/" ? "" : "/") + child->getName();
                string newChildPath = newPath + (newPath == "/" ? "" : "/") + child->getName();
                pathMap[newChildPath] = pathMap[oldChildPath];
                pathMap.erase(oldChildPath);
                updatePathMapRecursive(oldChildPath, newChildPath, child);
            }
        }
    }
   // We need to do erase all the sub folders inside our parent folder. After erasing all the subfolders then only we can erase our parent folder.
    void erasePathMapRecursive(string &currentPath, shared_ptr<Entity>&entity) {
         if(entity->isFolder()) {
            for(auto &child: entity->getChildren()) {
                string childPath = currentPath + (currentPath == "/"  ? "" : "/") + child->getName();
                erasePathMapRecursive(childPath, child);
            }
         }
         pathMap.erase(currentPath);
    }

    public:
    InMemoryFileSystem() {
        root = make_shared<Folder>("/");
        pathMap["/"] = root;
    }

    // READ OPERATION: Multiple threads can call this concurrently
    void list(string path) const {
        //shared lock allows concurrent reads
         shared_lock<shared_mutex> lock(fs_mutex);

         auto it = pathMap.find(path);
         if(it != pathMap.end() && it->second->isFolder()) {
            for(const auto& child: it->second->getChildren()) {
                cout<<child->getName() << child->isFolder() ? "/": ""<<"\n";
            }
         }
    }

    void createEntity(string path, bool isFile, int size = 0) {
        // uniqueue lock blocks all other readers and writers
        unique_lock<shared_mutex> lock(fs_mutex);

        if(pathMap.find(path) != pathMap.end()) {
            cout<<"Entity already exists. "<<"\n";
            return;
        }
        //split path will return a pair of strings. we are storing them into these two strings.
        auto[parentPath, entityName] = splitPath(path);

        auto it = pathMap.find(parentPath);

        if(it != pathMap.end() && it->second->isFolder()) {
           shared_ptr<Entity> newEntity = isFile ? make_shared<File>(entityName, size) : make_shared<Folder>(entityName);

           it->second->addChild(newEntity);
           pathMap[path] = newEntity;
        }
    }
    //WRITE OPERATION: Exclusive access required.
    void deleteEntity (string path) {
         unique_lock<shared_mutex> lock(fs_mutex);

         if(path == "/" || pathMap.find(path) == pathMap.end()) return;

         auto [parentPath, entityName] = splitPath(path);
         auto entity = pathMap[path];

         pathMap[parentPath]->removeChild(entity);

         //Since we deleted an entity we need to delete all it's entry from our pathMap, otherwise it will lead to inconsistency
         erasePathMapRecursive(path, entity);
    }

    void renameEntity(string path, string newName) {
         unique_lock<shared_mutex> lock(fs_mutex);

         if(path == "/" || pathMap.find(path) == pathMap.end()) return;

         auto [parentPath, entityName] = splitPath(path);
         auto entity = pathMap[path];
         //Rename to newName
         entity->setName(newName);
         //creating the newpath to store it in map. If it's not rot folder add an forward slash
         string newPath = parentPath + (parentPath == "/" ? "" : "/") + newName;
         //store the same entity in map with the newPath 
         pathMap[newPath] = entity;
         //delete the old path from the map
         pathMap.erase(path);
         //recursively delete all other paths from the map and rewrite them into newpath
         updatePathMapRecursive(path, newPath, entity);
    }

   
};
