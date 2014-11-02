// The MIT License (MIT)
// 
// Copyright (c) 2013 Taketoshi Aono(brn)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


namespace yatsc {

TypeRegistry::TypeRegistry() {}


TypeRegistry::~TypeRegistry() {}


void TypeRegistry::Register(Handle<ir::Node> node) {
  if (node->HasClassDeclView() ||
      node->HasInterfaceView() ||
      node->HasEnumDeclView() ||
      node->HasModuleDeclView() ||
      node->HasAmbientClassDeclarationView() ||
      node->HasAmbientEnumDeclarationView() ||
      node->HasAmbientModuleView()) {
    Handle<ir::Node> name = node->first_child();
    type_map_.insert(std::make_pair(name->string_value()->utf16_value(), node));
  }
}


void TypeRegistry::RegisterExernalPhaiType(Handle<ir::Node> external_type) {
  if (external_type->HasNameView()) {
    type_map_.insert(std::make_pair(external_type->string_value()->utf16_value(), external_type));
  }
}


Handle<ir::Node> TypeRegistry::FindType(const Utf16String& name) {
  return type_map_.find(name)->second;
}

}
