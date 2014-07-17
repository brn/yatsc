### <a name="Module"> Module
- [ModuleBody](#ModuleBody)

### <a name="ModuleBody"> ModuleBody
- [ModuleItemList](#ModuleItemList)

### <a name="ModuleItemList"> ModuleItemList
- [ModuleItem](#ModuleItem)
- [MouduleItemList](#MouduleItemList) [ModuleItem](#ModuleItem)

### <a name="ModuleItem"> ModuleItem
- [ExternalImportDeclaration](#ExternalImportDeclaration)
- [ImportDeclaration](#ImportDeclaration)
- [ExportDeclaration](#ExportDeclaration)
- [StatementListItem](#StatementListItem)

### ExternalImportDeclaration
- ___import___ [Identifier](./EXPRESSION_SYNTAX_GRAMMAR.md#Identifier) ___=___ [ExternalModuleReference](#ExternalModuleReference) ___;___

### <a name="ExternalModuleReference"> ExternalModuleReference
- ___require___ ___(___ [StringLiteral](./EXPRESSION_SYNTAX_GRAMMAR.md#StringLiteral) ___)___

### <a name="ImportDeclaration"> ImportDeclaration
- [ModuleImport](#ModuleImport)
- ___import___ [ImportClause](#ImportClause) [FromClause](#FromClause)
- ___import___ [ModuleSpecifier](#ModuleSpecifier)

### <a name="ModuleImport"> ModuleImport
- ___module___ __[no Line Terminator here]__ [ImportBinding](#ImportBinding) [FromClause](#FromClause) ___;___

### <a name="FromClause"> FromClause
- ___from___ [ModuleSpecifier](#ModuleSpecifier)

### <a name="ImportClause"> ImportClause
- [ImportedBinding](#ImportedBinding)
- [ImportedBinding](#ImportedBinding) ___,___ [NamedImports](#NamedImports)
- [NamedImports](#NamedImports)

### <a name="NamedImports"> NamedImports
- ___{___ ___}___
- ___{___ [ImportsList](#ImportsList) ___}___
- ___{___ [ImportsList](#ImportsList) ___,___ ___}___

### ImportsList
- [ImportSpecifier](#ImportSpecifier)
- [ImportsList](#ImportsList) ___,___ [ImportSpecifier](#ImportSpecifier)

### <a name="ImportSpecifier"> ImportSpecifier
- [ImportedBinding](#ImportedBinding)
- [IdentifierName](./EXPRESSION_SYNTAX_GRAMMAR.md#IdentifierName) ___as___ [ImportedBinding](#ImportedBinding)

### <a name="ModuleSpecifier"> ModuleSpecifier
- [StringLiteral](./EXPRESSION_SYNTAX_GRAMMAR.md#StringLiteral)

### <a name="ImportedBinding"> ImportedBinding
- [BindingIdentifier](./EXPRESSION_SYNTAX_GRAMMAR.md#BindingIdentifier)

### <a name="ExportDeclaration"> ExportDeclaration
- ___export___ ___*___ [FromClause](#FromClause) ___;___
- ___export___ [ExportClause\[NoReference\]](#ExportClause) [FromClause](#FromClause) ___;___
- ___export___ [ExportClause](#ExportClause) ___;___
- ___export___ [VariableStatement](#VariableStatement)
- ___export___ [Declaration\[Default\]](#Declaration)
- ___export___ ___default___ [AssignmentExpression\[In\]](#AssignmentExpression) ___;___
- ___export___ ___=___ [Identifier](./EXPRESSION_SYNTAX_GRAMMAR.md#Identifier) ___;___

### <a name="ExportClause"> ExportClause[NoReference]
- ___{___ ___}___
- ___{___ [ExportsList\[?NoReference\]](#ExportsList) ___}___
- ___{___ [ExportsList\[?NoReference\]](#ExportsList) ___,___ ___}___

### <a name="ExportsList"> ExportsList[NoReference]
- [ExportSpecifier\[?NoReference\]](#ExportSpecifier)
- [ExportsList\[?NoReference\]](#ExportsList) ___,___ [ExportSpecifier\[?NoReference\]](#ExportSpecifier)

### <a name="ExportSpecifier"> ExportSpecifier[NoReference]
- [~NoReference] [IdentifierReference](#IdentifierReference)
- [~NoReference] [IdentifierReference](#IdentifierReference) ___as___ [IdentifierName](#IdentifierName)
- [+NoReference] [IdentifierName](./EXPRESSION_SYNTAX_GRAMMAR.md#IdentifierName)
- [+NoReference] [IdentifierName](./EXPRESSION_SYNTAX_GRAMMAR.md#IdentifierName) ___as___ [IdentifierName](#IdentifierName)

### <a name="ImportDeclaration"> ImportDeclaration
- ___import___ [Identifier](./EXPRESSION_SYNTAX_GRAMMAR.md#Identifier) ___=___ [EntityName](#EntityName) ___;___

### <a name="EntityName"> EntityName
- [Identifier](./EXPRESSION_SYNTAX_GRAMMAR.md#Identifier)
- [ModuleName](#ModuleName) ___.___ [Identifier](./EXPRESSION_SYNTAX_GRAMMAR.md#Identifier)

### <a name="ModuleDeclaration"> ModuleDeclaration[TsModule]
- \[+TsModule\] ___module___ [IdentifierPath](#IdentifierPath) ___{___ [ModuleBody](#ModuleBody) ___}___

### <a name="IdentifierPath"> IdentifierPath
- [Identifier](./EXPRESSION_SYNTAX_GRAMMAR.md#Identifier)
- [IdentifierPath](#IdentifierPath) ___.___ [Identifier](./EXPRESSION_SYNTAX_GRAMMAR.md#Identifier)

### <a name="ModuleBody"> ModuleBody
- [ModuleElements\(opt\)](#ModuleElements)

### <a name="ModuleElements"> ModuleElements
- [ModuleElement](#ModuleElement)
- [ModuleElements](#ModuleElements) [ModuleElement](#ModuleElement)

### <a name="ModuleElement"> ModuleElement
- [Statement](#Statement)
- ___export\(opt\)___ [VariableDeclaration](#VariableDeclaration)
- ___export\(opt\)___ [FunctionDeclaration](#FunctionDeclaration)
- ___export\(opt\)___ [ClassDeclaration](#ClassDeclaration)
- ___export\(opt\)___ [InterfaceDeclaration](#InterfaceDeclaration)
- ___export\(opt\)___ [EnumDeclaration](#EnumDeclaration)
- ___export\(opt\)___ [ModuleDeclaration](#ModuleDeclaration)
- ___export\(opt\)___ [ImportDeclaration](#ImportDeclaration)

### <a name="ImportDeclaration"> ImportDeclaration
- ___import___ [Identifier](./EXPRESSION_SYNTAX_GRAMMAR.md#Identifier) ___=___ [EntityName](#EntityName) ___;___

### <a name="EntityName"> EntityName
- [Identifier](./EXPRESSION_SYNTAX_GRAMMAR.md#Identifier)
- [ModuleName](#ModuleName) ___.___ [Identifier](./EXPRESSION_SYNTAX_GRAMMAR.md#Identifier)
