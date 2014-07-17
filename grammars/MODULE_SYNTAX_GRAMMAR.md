### <a name="ModuleDeclaration"> Module
- [ModuleBody](#ModuleBody)

### <a name="ModuleBody"> ModuleBody
- [ModuleItemList](#ModuleItemList)

### <a name="ModuleItemList"> ModuleItemList
- [ModuleItem](#ModuleItem)
- [MouduleItemList](#MouduleItemList) [ModuleItem](#ModuleItem)

### <a name="ModuleItem"> ModuleItem
- [ImportDeclaration](#ImportDeclaration)
- [ExportDeclaration](#ExportDeclaration)
- [StatementListItem](#StatementListItem)

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
- [IdentifierName](#IdentifierName) ___as___ [ImportedBinding](#ImportedBinding)

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
- [+NoReference] [IdentifierName](#IdentifierName)
- [+NoReference] [IdentifierName](#IdentifierName) ___as___ [IdentifierName](#IdentifierName)

### <a name="ImportDeclaration"> ImportDeclaration
- ___import___ [Identifier](#Identifier) ___=___ [EntityName](#EntityName) ___;___

### <a name="EntityName"> EntityName
- [Identifier](#Identifier)
- [ModuleName](#ModuleName) ___.___ [Identifier](#Identifier)
