### <a name="AmbientDeclaration"> AmbientDeclaration
- ___declare___ [AmbientVariableDeclaration](#AmbientVariableDeclaration)
- ___declare___ [AmbientFunctionDeclaration](#AmbientFunctionDeclaration)
- ___declare___ [AmbientClassDeclaration](#AmbientClassDeclaration)
- ___declare___ [AmbientEnumDeclaration](#AmbientEnumDeclaration)
- ___declare___ [AmbientModuleDeclaration](#AmbientModuleDeclaration)
- ___declare___ [AmbientExternalModuleDeclaration](#AmbientExternalModuleDeclaration)

### <a name="AmbientVariableDeclaration"> AmbientVariableDeclaration
- ___var___ [Identifier](#Identifier) [TypeAnnotation\(opt\)](#TypeAnnotation) ___;___

### <a name="AmbientFunctionDeclaration"> AmbientFunctionDeclaration
- ___function___ [Identifier](#Identifier) [CallSignature](#CallSignature) ___;___

### <a name="AmbientClassDeclaration"> AmbientClassDeclaration
- ___class___ [Identifier](#Identifier) [TypeParameters\(opt\)](#TypeParameters) [ClassHeritage](#ClassHeritage) ___{___ [AmbientClassBody](#AmbientClassBody) ___}___

### <a name="AmbientClassBody"> AmbientClassBody
- [AmbientClassBodyElements\(opt\)](#AmbientClassBodyElements)

### <a name="AmbientClassBodyElements"> AmbientClassBodyElements
- [AmbientClassBodyElement](#AmbientClassBodyElement)
- [AmbientClassBodyElements](#AmbientClassBodyElements) [AmbientClassBodyElement](#AmbientClassBodyElement)

### <a name="AmbientClassBodyElement"> AmbientClassBodyElement
- [AmbientConstructorDeclaration](#AmbientConstructorDeclaration)
- [AmbientPropertyMemberDeclaration](#AmbientPropertyMemberDeclaration)
- [IndexSignature](#IndexSignature)

### <a name="AmbientConstructorDeclaration"> AmbientConstructorDeclaration
- ___constructor___ ___(___ [ParameterList\(opt\)](#ParameterList) ___)___ ___;___

### <a name="AmbientPropertyMemberDeclaration"> AmbientPropertyMemberDeclaration
- [PublicOrPrivateopt](#PublicOrPrivateopt) ___static\(opt\)___ [PropertyName](#PropertyName) [TypeAnnotationopt](#TypeAnnotationopt) ___;___
- [PublicOrPrivateopt](#PublicOrPrivateopt) ___static\(opt\)___ [PropertyName](#PropertyName) [CallSignature](#CallSignature) ___;___

### <a name="AmbientEnumDeclaration"> AmbientEnumDeclaration
- ___enum___ [Identifier](#Identifier) ___{___ [AmbientEnumBody\(opt\)](#AmbientEnumBody) ___}___

### <a name="AmbientEnumBody"> AmbientEnumBody
- [AmbientEnumMemberList](#AmbientEnumMemberList) ___,___\(opt\)

### <a name="AmbientEnumMemberList"> AmbientEnumMemberList
- [AmbientEnumMember](#AmbientEnumMember)
- [AmbientEnumMemberList](#AmbientEnumMemberList) ___,___ [AmbientEnumMember](#AmbientEnumMember)

### <a name="AmbientEnumMember"> AmbientEnumMember
- [PropertyName](#PropertyName)
- [PropertyName](#PropertyName) ___=___ [NumericLiteral](#NumericLiteral)

### <a name="AmbientModuleDeclaration"> AmbientModuleDeclaration
- ___module___ [IdentifierPath](#IdentifierPath) ___{___ [AmbientModuleBody](#AmbientModuleBody) ___}___

### <a name="AmbientModuleBody"> AmbientModuleBody
- [AmbientModuleElements\(opt\)](#AmbientModuleElements)

### <a name="AmbientModuleElements"> AmbientModuleElements
- [AmbientModuleElement](#AmbientModuleElement)
- [AmbientModuleElements](#AmbientModuleElements) [AmbientModuleElement](#AmbientModuleElement)

### <a name="AmbientModuleElement"> AmbientModuleElement
- ___export\(opt\)___ [AmbientVariableDeclaration](#AmbientVariableDeclaration)
- ___export\(opt\)___ [AmbientFunctionDeclaration](#AmbientFunctionDeclaration)
- ___export\(opt\)___ [AmbientClassDeclaration](#AmbientClassDeclaration)
- ___export\(opt\)___ [InterfaceDeclaration](#InterfaceDeclaration)
- ___export\(opt\)___ [AmbientEnumDeclaration](#AmbientEnumDeclaration)
- ___export\(opt\)___ [AmbientModuleDeclaration](#AmbientModuleDeclaration)
- ___export\(opt\)___ [ImportDeclaration](#ImportDeclaration)

### <a name="AmbientExternalModuleDeclaration"> AmbientExternalModuleDeclaration
- ___module___ [StringLiteral](#StringLiteral) ___{___ [AmbientExternalModuleBody](#AmbientExternalModuleBody) ___}___

### <a name="AmbientExternalModuleBody"> AmbientExternalModuleBody
- [AmbientExternalModuleElements\(opt\)](#AmbientExternalModuleElements)

### <a name="AmbientExternalModuleElements"> AmbientExternalModuleElements
- [AmbientExternalModuleElement](#AmbientExternalModuleElement)
- [AmbientExternalModuleElements](#AmbientExternalModuleElements) [AmbientExternalModuleElement](#AmbientExternalModuleElement)

### <a name="AmbientExternalModuleElement"> AmbientExternalModuleElement
- [AmbientModuleElement](#AmbientModuleElement)
- ___export___ ___=___ [Identifier](./EXPRESSION_SYNTAX_GRAMMAR.md#Identifier) ___;___
- ___export\(opt\)___ [ExternalImportDeclaration](#ExternalImportDeclaration)
