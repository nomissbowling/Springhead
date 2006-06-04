#include <Springhead.h>
#include <Foundation/Object.h>
#include "Scene.h"
#include <sstream>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif


namespace Spr{;

UTTypeInfoImp<ObjectNames::ObjectKey> ObjectNames::ObjectKey::typeInfo("", NULL);
ObjectNames::ObjectKey ObjectNames::key;
ObjectNames::ObjectKey::ObjectKey(){
	AddRef();
}
ObjectNames::ObjectKey::~ObjectKey(){
	DelRef();
}
bool ObjectNames::Add(NamedObject* obj){
	if (!obj->GetName() || !strlen(obj->GetName())) return false;
	std::pair<iterator, bool> rv = insert(obj);
	if (rv.second){
		return true;
	}else if (*rv.first == obj){
		return false;
	}
	UTString base = obj->name;
	int i=1;
	do{
		std::ostringstream ss;
		ss << "_" << i << '\0';
		obj->name = base + ss.str();
		rv = insert(obj);
		i++;
	} while(!rv.second);
	nameMap[base] = obj->name;
	DSTR << "name change: " << base << " -> " << obj->name << std::endl;
	return true;
}
void ObjectNames::Print(std::ostream& os) const{
	int w = os.width();
	os.width(0);
	for(const_iterator it = begin(); it != end(); ++it){
		os << UTPadding(w);
		os << (*it)->GetName() << " : " << (*it)->GetTypeInfo()->ClassName();
		os << std::endl;
	}
	os.width(w);
}
bool ObjectNamesLess::operator () (const NamedObject* o1, const NamedObject* o2) const {
	int name = strcmp(o1->GetName(), o2->GetName());
	if (name < 0) return true;
/*	if (name == 0){
		int cls = 0;
		if (GETCLASSNAME(o1)[0] && GETCLASSNAME(o2)[0]){
			cls = strcmp(GETCLASSNAME(o1), GETCLASSNAME(o2));
			if (cls < 0) return true;
		}
	}
*/
	return false;
}
//----------------------------------------------------------------------------
//	NameManager
IF_OBJECT_IMP(NameManager, NamedObject);
NameManager NameManager::theRoot;

NameManager::NameManager(){
}
void NameManager::SetNameManager(NameManager* p){
	if (nameManager==p) return;
	if (nameManager) nameManager->DelChildManager(this);
	if (p) p->AddChildManager(this);
}
void NameManager::AddChildManager(NameManager* c){
	if (c->nameManager == this) return;
	c->SetNameManager(NULL);
	c->nameManager = this;
	childManagers.push_back(c);
}
void NameManager::DelChildManager(NameManager* c){
	if (c->nameManager != this) return;
	c->nameManager = NULL;
	NameManagers::iterator it = std::find(childManagers.begin(), childManagers.end(), c);
	assert( it!=childManagers.end() );
	childManagers.erase(it);
}
void NameManager::ClearName(){
	names.clear();
	for(NameManagers::iterator it = childManagers.begin(); it != childManagers.end(); ++it){
		(*it)->ClearName();
	}
}
void NameManager::Print(std::ostream& os) const {
	int w = os.width();
	os.width(0);
	os << UTPadding(w);
	os << "<" << GetTypeInfo()->ClassName() << " " << name.c_str() << ">" << std::endl;
	os.width(w+2);
	names.Print(os); 
	for(size_t i=0; i<NChildObject(); ++i){
		GetChildObject(i)->Print(os);
	}
	os.width(0);
	os << UTPadding(w);
	os << "</" << GetTypeInfo()->ClassName() << ">" << std::endl;
	os.width(w);
}

NamedObjectIf* NameManager::FindObjectExact(UTString name, const char* cls){
	NamedObjectIf* rv = NULL;
	int pos = name.find('/');
	if (pos != UTString::npos){	//	 名前空間の指定がある場合
		UTString n = name.substr(pos+1);
		UTString ns = name.substr(0, pos);
		//	ぴったりのものを探す．
		for(NameManagers::iterator it = childManagers.begin(); it != childManagers.end(); ++it){
			if (ns.compare((*it)->GetName()) == 0){
				rv = (*it)->FindObjectExact(n, cls);
				if (rv) return rv;
			}
		}
	}else{	//	無い場合
		//	ぴったりのものを探す
		rv = names.Find(name, cls);
	}
	return rv;
}
NamedObjectIf* NameManager::FindObject(UTString name, const char* cls){
	NamedObjectIf* rv = NULL;
	int pos = name.find('/');
	if (pos != UTString::npos){	//	 名前空間の指定がある場合
		UTString n = name.substr(pos+1);
		UTString ns = name.substr(0, pos);
		//	ぴったりのものを探す．
		for(NameManagers::iterator it = childManagers.begin(); it != childManagers.end(); ++it){
			if (ns.compare((*it)->GetName()) == 0){
				rv = (*it)->FindObjectExact(n, cls);
				if (rv) return rv;
			}
		}
		//	なければ子孫についてぴったりのものを探す．
		for(NameManagers::iterator it = childManagers.begin(); it != childManagers.end(); ++it){
			rv = (*it)->FindObjectExact(name, cls);
			if (rv) return rv;
		}
		//	なければ祖先について探す
		NameManager* nm = nameManager;
		while(nm){
			rv = nm->FindObject(name, cls);
			if (rv) return rv;
			nm = nm->nameManager;
		}
		//	それでもない場合は名前空間を短くして探す．
		rv = FindObject(n, cls);
		if (rv) return rv;
	}else{	//	 名前空間の指定がない場合
		//	ぴったりのものを探す
		rv = names.Find(name, cls);
		if (rv) return rv;
		//	なければ，子孫について探す
		for(NameManagers::iterator it = childManagers.begin(); it != childManagers.end(); ++it){
			rv = (*it)->FindObject(name, cls);
			if (rv) return rv;
		}
		//	なければ祖先について探す
		NameManager* nm = nameManager;
		while(nm){
			rv = nm->FindObject(name, cls);
			if (rv) return rv;
			nm = nm->nameManager;
		}
	}
	return rv;
}


//----------------------------------------------------------------------------
//	Scene
IF_OBJECT_IMP(Scene, NameManager);

Scene::Scene(){
	Clear();
}
void Scene::Clear(){
	ClearName();
}

}
