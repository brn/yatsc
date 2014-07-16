## TypeParameters
- ___<___ [TypeParameterList](#TypeParameterList) ___>___

### <a name="TypeParameterList"> TypeParameterList
- [TypeParameter](#TypeParameter)
- [TypeParameterList](#TypeParameterList) ___,___ [TypeParameter](#TypeParameter)

### <a name="TypeParameter"> TypeParameter
- [Identifier](#Identifier) [Constraint\(opt\)](#Constraint)

### <a name="Constraint"> Constraint
- ___extends___ [Type](#Type)

### <a name="Type"> Type
- [PredefinedType](#PredefinedType)
- [TypeReference](#TypeReference) [TypeQuery](#TypeQuery) [TypeLiteral](#TypeLiteral)

### <a name="PredefinedType"> PredefinedType:
   ___any___
   ___number___
   ___boolean___
   ___string___
   ___void___
   
### <a name="TypeReference"> TypeReference
- [TypeName](#TypeName) \[no [LineTerminator](#LineTerminator) here\] [TypeArguments\(opt\)](#TypeArguments)

### <a name="TypeName"> TypeName
- [Identifier](#Identifier)
- [ModuleName](#ModuleName) ___.___ [Identifier](#Identifier)

### <a name="ModuleName"> ModuleName
- [Identifier](#Identifier)
- [ModuleName](#ModuleName) ___.___ [Identifier](#Identifier)

### <a name="TypeArguments"> TypeArguments
- ___<___ [TypeArgumentList](#TypeArgumentList) ___>___

### <a name="TypeArgumentList"> TypeArgumentList
- [TypeArgument](#TypeArgument)
- [TypeArgumentList](#TypeArgumentList) ___,___ [TypeArgument](#TypeArgument)

### <a name="TypeArgument"> TypeArgument
- [Type](#Type)

### <a name="TypeQuery"> TypeQuery
- ___typeof___ [TypeQueryExpression](#TypeQueryExpression)

### <a name="TypeQueryExpression"> TypeQueryExpression
- [Identifier](#Identifier)
- [TypeQueryExpression](#TypeQueryExpression) ___.___ [IdentifierName](#IdentifierName)

### <a name="TypeLiteral"> TypeLiteral
- [ObjectType](#ObjectType)
- [ArrayType](#ArrayType)
- [FunctionType](#FunctionType)
- [ConstructorType](#ConstructorType)

### <a name="ArrayType"> ArrayType
- [ElementType](#ElementType) \[no LineTerminator here\] ___\[___ ___\]___

### <a name="ElementType"> ElementType
- [PredefinedType](#PredefinedType)
- [TypeReference](#TypeReference)
- [TypeQuery](#TypeQuery)
- [ObjectType](#ObjectType)
- [ArrayType](#ArrayType)

### <a name="FunctionType"> FunctionType
- [TypeParameters\(opt\)](#TypeParameters) ___(___ [FormalParameterList\(opt\)](#FormalParameterList) ___)___ ___=>___ [Type](#Type)

### <a name="ConstructorType"> ConstructorType
- ___new___ [TypeParameters\(opt\)](#TypeParameters) ___(___ [FormalParameterList\(opt\)](#FormalParameterList) ___)___ ___=>___ [Type](#Type)

### <a name="ObjectType"> ObjectType
- ___{___ [TypeBody\(opt\)](#TypeBody) ___}___

### <a name="TypeBody"> TypeBody
- [TypeMemberList](#TypeMemberList) ___;___\(opt\)

### <a name="TypeMemberList"> TypeMemberList
- [TypeMember](#TypeMember)
- [TypeMemberList](#TypeMemberList) ___;___ [TypeMember](#TypeMember)

### <a name="TypeMember"> TypeMember
- [PropertySignature](#PropertySignature)
- [CallSignature](#CallSignature)
- [ConstructSignature](#ConstructSignature)
- [IndexSignature](#IndexSignature)
- [MethodSignature](#MethodSignature)

### <a name="PropertySignature"> PropertySignature
- [PropertyName](#PropertyName) ___?___\(opt\) [TypeAnnotation\(opt\)](#TypeAnnotation)

### <a name="PropertyName"> PropertyName
- [IdentifierName](#IdentifierName)
- [StringLiteral](#StringLiteral)
- [NumericLiteral](#NumericLiteral)

### <a name="CallSignature"> CallSignature:
- [TypeParameters\(opt\)](#TypeParameters) ___(___ [FormalParameterList\(opt\)](./STATEMENT_GRAMMAR#FormalParameterList) ___)___ [TypeAnnotation\(opt\)](#TypeAnnotation)

### <a name="[ConstructSignature](#ConstructSignature)"> [ConstructSignature](#ConstructSignature)
- ___new___ [TypeParameters\(opt\)](#TypeParameters) ___(___ [FormalParameterList\(opt\)](#FormalParameterList\) ___)___ [TypeAnnotation\(opt\)](#TypeAnnotation)

### <a name="IndexSignature"> IndexSignature
- ___\[___ [Identifier](#Identifier) ___:___ ___string___ ___\]___ [TypeAnnotation](#TypeAnnotation)
- ___\[___ [Identifier](#Identifier) ___:___ ___number___ ___\]___ [TypeAnnotation](#TypeAnnotation)

### <a name="MethodSignature"> MethodSignature
- [PropertyName](#PropertyName) ___?___\(opt\) [CallSignature](#CallSignature)
