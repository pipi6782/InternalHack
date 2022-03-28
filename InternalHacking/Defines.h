#pragma once

#define SafeDelete(p) {if(p != nullptr) delete p; p=nullptr;}
#define SafeDeleteArray(p) {if(p != nullptr) delete[] p; p=nullptr;}
#define SafeRelease(p) {if(p != nullptr) p->Release(); p=nullptr;}

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define Check(p) assert(SUCCEEDED(p));