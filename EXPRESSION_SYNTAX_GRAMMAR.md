# Expression  
### <a name="Expression"> Expression\[In, Yield]  
- [AssignmentExpression\[?In, ?Yield\]](#AssignmentExpression)  
- [Expression\[?In, ?Yield\]](#Expression) ___,___ [AssignmentExpression\[?In, ?Yield\]](#AssignmentExpression)  

### <a name="AssignmentPattern"> AssignmentPattern\[Yield]  
- [ObjectAssignmentPattern\[?Yield\]](#ObjectAssignmentPattern)  
- [ArrayAssignmentPattern\[?Yield\]](#ArrayAssignmentPattern)  

### <a name="ObjectAssignmentPattern"> ObjectAssignmentPattern\[Yield]  
- ___{___ ___}___  
- ___{___ [AssignmentPropertyList\[?Yield\]](#AssignmentPropertyList) ___}___  
- ___{___ [AssignmentPropertyList\[?Yield\]](#AssignmentPropertyList) , ___}___  

### <a name="ArrayAssignmentPattern"> ArrayAssignmentPattern\[Yield]  
- ___\[___ [Elision\(opt\)](#Elision) [AssignmentRestElement\[?Yield\]\(opt\)](#AssignmentRestElement) ___]___  
- ___\[___ [AssignmentElementList\[?Yield\]](#AssignmentElementList) ___]___  
- ___\[___ [AssignmentElementList\[?Yield\]](#AssignmentElementList) , [Elision\(opt\)](#Elision) [AssignmentRestElement\[?Yield\]\(opt\)](#AssignmentRestElement) ___]___  

### <a name="AssignmentPropertyList"> AssignmentPropertyList\[Yield]  
- [AssignmentProperty\[?Yield\]](#AssignmentProperty)  
- [AssignmentPropertyList\[?Yield\]](#AssignmentPropertyList) ___,___ [AssignmentProperty\[?Yield\]](#AssignmentProperty)  

### <a name="AssignmentElementList"> AssignmentElementList\[Yield]  
- [AssignmentElisionElement\[?Yield\]](#AssignmentElisionElement)  
- [AssignmentElementList\[?Yield\]](#AssignmentElementList) ___,___ [AssignmentElisionElement\[?Yield\]](#AssignmentElisionElement)  

### <a name="AssignmentElisionElement"> AssignmentElisionElement\[Yield]  
- [Elision\(opt\)](#Elision) [AssignmentElement\[?Yield\]](#AssignmentElement)  

### <a name="AssignmentProperty"> AssignmentProperty\[Yield] :  
- [IdentifierReference\[?Yield\]](#IdentifierReference) [Initializer\[In,?Yield\]\(opt\)](#Initializer)  
- [PropertyName](#PropertyName) ___:___ [AssignmentElement\[?Yield\]](#AssignmentElement)  

### <a name="AssignmentElement"> AssignmentElement\[Yield]  
- [DestructuringAssignmentTarget\[?Yield\]](#DestructuringAssignmentTarget) [Initializer\[In,?Yield\]\(opt\)](#Initializer)  

### <a name="AssignmentRestElement"> AssignmentRestElement\[Yield]  
- ___...___ [DestructuringAssignmentTarget\[?Yield\]](#DestructuringAssignmentTarget)  

### <a name="DestructuringAssignmentTarget"> DestructuringAssignmentTarget\[Yield]  
- [LeftHandSideExpression\[?Yield\]](#LeftHandSideExpression)  

### <a name="AssignmentExpression"> AssignmentExpression\[In, Yield]  
- [ConditionalExpression\[?In, ?Yield\]](#ConditionalExpression)  
- \[+Yield\] [YieldExpression\[?In\]](#YieldExpression)  
- [ArrowFunction\[?In, ?Yield\]](#ArrowFunction)  
- [LeftHandSideExpression\[?Yield\]](#LeftHandSideExpression) ___=___ [AssignmentExpression\[?In,?Yield\]](#AssignmentExpression)  
- [LeftHandSideExpression\[?Yield\]](#LeftHandSideExpression) [AssignmentOperator](#AssignmentOperator) [AssignmentExpression\[?In, ?Yield\]](#AssignmentExpression)  

### <a name="AssignmentOperator"> AssignmentOperator  
- *one of* `*= /= %= += -= <<= >>= >>>= &= ^= |=`  

### <a name="ConditionalExpression"> ConditionalExpression\[In, Yield]  
- [LogicalORExpression\[?In, ?Yield\]](#LogicalORExpression)  
- [LogicalORExpression\[?In,?Yield\]](#LogicalORExpression) ___?___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression) ___:___ [AssignmentExpression\[?In, ?Yield\]](#AssignmentExpression)  

### <a name="BitwiseORExpression"> BitwiseORExpression\[In, Yield]  
- [BitwiseXORExpression\[?In, ?Yield\]](#BitwiseXORExpression)  
- [BitwiseORExpression\[?In, ?Yield\]](#BitwiseORExpression) ___|___ [BitwiseXORExpression\[?In, ?Yield\]](#BitwiseXORExpression)  

### <a name="BitwiseXORExpression"> BitwiseXORExpression\[In, Yield]  
- [BitwiseANDExpression\[?In, ?Yield\]](#BitwiseANDExpression)  
- [BitwiseXORExpression\[?In, ?Yield\]](#BitwiseXORExpression) ___^___ [BitwiseANDExpression\[?In, ?Yield\]](#BitwiseANDExpression)  

### <a name="BitwiseANDExpression"> BitwiseANDExpression\[In, Yield]  
- [EqualityExpression\[?In, ?Yield\]](#EqualityExpression)  
- [BitwiseANDExpression\[?In, ?Yield\]](#BitwiseANDExpression) ___&___ [EqualityExpression\[?In, ?Yield\]](#EqualityExpression)  

### <a name="EqualityExpression"> EqualityExpression\[In, Yield]  
- [RelationalExpression\[?In, ?Yield\]](#RelationalExpression)  
- [EqualityExpression\[?In, ?Yield\]](#EqualityExpression) ___==___ [RelationalExpression\[?In, ?Yield\]](#RelationalExpression)  
- [EqualityExpression\[?In, ?Yield\]](#EqualityExpression) ___!=___ [RelationalExpression\[?In, ?Yield\]](#RelationalExpression)  
- [EqualityExpression\[?In, ?Yield\]](#EqualityExpression) ___===___[RelationalExpression\[?In, ?Yield\]](#RelationalExpression)  
- [EqualityExpression\[?In, ?Yield\]](#EqualityExpression) ___!==___[RelationalExpression\[?In, ?Yield\]](#RelationalExpression)  

### <a name="RelationalExpression"> RelationalExpression\[In, Yield]  
- [ShiftExpression\[?Yield\]](#ShiftExpression)  
- [RelationalExpression\[?In, ?Yield\]](#RelationalExpression ___<___ [ShiftExpression\[?Yield\]](#ShiftExpression)  
- [RelationalExpression\[?In, ?Yield\]](#RelationalExpression ___>___ [ShiftExpression\[?Yield\]](#ShiftExpression)  
- [RelationalExpression\[?In, ?Yield\]](#RelationalExpression ___<=___ [ShiftExpression\[?Yield\]](#ShiftExpression)  
- [RelationalExpression\[?In, ?Yield\]](#RelationalExpression ___>=___ [ShiftExpression\[?Yield\]](#ShiftExpression)  
- [RelationalExpression\[?In, ?Yield\]](#RelationalExpression ___instanceof___ [ShiftExpression\[?Yield\]](#ShiftExpression)  
- \[+In\] [RelationalExpression\[In, ?Yield\]](#RelationalExpression) ___in___ [ShiftExpression\[?Yield\]](#ShiftExpression)  

### <a name="ShiftExpression"> ShiftExpression\[Yield]  
- [AdditiveExpression\[?Yield\]](#AdditiveExpression)  
- [ShiftExpression\[?Yield\]](#ShiftExpression) ___<<___ [AdditiveExpression\[?Yield\]](#AdditiveExpression)  
- [ShiftExpression\[?Yield\]](#ShiftExpression) ___>>___ [AdditiveExpression\[?Yield\]](#AdditiveExpression)  
- [ShiftExpression\[?Yield\]](#ShiftExpression) ___>>>___ [AdditiveExpression\[?Yield\]](#AdditiveExpression)  

### <a name="AdditiveExpression"> AdditiveExpression\[Yield]  
- [MultiplicativeExpression\[?Yield\]](#MultiplicativeExpression)  
- [AdditiveExpression\[?Yield\]](#AdditiveExpression) ___+___ [MultiplicativeExpression\[?Yield\]](#MultiplicativeExpression)  
- [AdditiveExpression\[?Yield\]](#AdditiveExpression) ___-___ [MultiplicativeExpression\[?Yield\]](#MultiplicativeExpression)  

### <a name="MultiplicativeExpression"> MultiplicativeExpression\[Yield]  
- [UnaryExpression\[?Yield\]](#UnaryExpression)  
- [MultiplicativeExpression\[?Yield\]](#MultiplicativeExpression) ___*___ [UnaryExpression\[?Yield\]](#UnaryExpression)  
- [MultiplicativeExpression\[?Yield\]](#MultiplicativeExpression) ___/___ [UnaryExpression\[?Yield\]](#UnaryExpression)  
- [MultiplicativeExpression\[?Yield\]](#MultiplicativeExpression) ___%___ [UnaryExpression\[?Yield\]](#UnaryExpression)  

### <a name="UnaryExpression"> UnaryExpression\[Yield]  
- [PostfixExpression\[?Yield\]](#PostfixExpression)  
- ___delete___ [UnaryExpression\[?Yield\]](#UnaryExpression)  
- ___void___ [UnaryExpression\[?Yield\]](#UnaryExpression)  
- ___typeof___ [UnaryExpression\[?Yield\]](#UnaryExpression)  
- ___++___ [UnaryExpression\[?Yield\]](#UnaryExpression)  
- ___--___ [UnaryExpression\[?Yield\]](#UnaryExpression)  
- ___+___ [UnaryExpression\[?Yield\]](#UnaryExpression)  
- ___-___ [UnaryExpression\[?Yield\]](#UnaryExpression)  
- ___~___ [UnaryExpression\[?Yield\]](#UnaryExpression)  
- ___!___ [UnaryExpression\[?Yield\]](#UnaryExpression)  

### <a name="PostfixExpression"> PostfixExpression\[Yield]  
- [LeftHandSideExpression\[?Yield\]](#LeftHandSideExpression)  
- [LeftHandSideExpression\[?Yield\]](#LeftHandSideExpression) \[no LineTerminator here\] ___++___  
- [LeftHandSideExpression\[?Yield\]](#LeftHandSideExpression) \[no LineTerminator here\] ___--___  

### <a name="MemberExpression"> MemberExpression\[Yield]  
- \[Lexical goal InputElementRegExp\] [PrimaryExpression\[?Yield\]](#PrimaryExpression)  
- [MemberExpression\[?Yield\]](#MemberExpression) ___\[___ [Expression\[In, ?Yield\]](#Expression) ___]___  
- [MemberExpression\[?Yield\]](#MemberExpression) ___.___ [IdentifierName](#IdentifierName)  
- [MemberExpression\[?Yield\]](#MemberExpression) [TemplateLiteral\[?Yield\]](#TemplateLiteral)  
- ___super___ \[ [Expression\[In, ?Yield\]](#Expression) ]  
- ___super___ ___.___ [IdentifierName](#IdentifierName)  
- ___new___ ___super___ [Arguments\[?Yield\]\(opt\)](#Arguments)  
- ___new___ ___\[___ *lookahead*  ___{___ ___super___ ___}___ ___]___ [MemberExpression\[?Yield\]](#MemberExpression)  

### <a name="NewExpression"> NewExpression\[Yield]  
- [MemberExpression\[?Yield\]](#MemberExpression)  
- ___new___ [NewExpression\[?Yield\]](#NewExpression)  
- ___new___ ___super___  

### <a name="CallExpression"> CallExpression\[Yield]  
- [MemberExpression\[?Yield\]](#MemberExpression) [Arguments\[?Yield\]](#Arguments)  
- ___super___ [Arguments\[?Yield\]](#Arguments)  
- [CallExpression\[?Yield\]](#CallExpression) [Arguments\[?Yield\]](#Arguments)  
- [CallExpression\[?Yield\]](#CallExpression) ___\[___ [Expression\[In, ?Yield\]](#Expression) ___]___  
- [CallExpression\[?Yield\]](#CallExpression) ___.___ [IdentifierName](#IdentifierName)  
- [CallExpression\[?Yield\]](#CallExpression) [TemplateLiteral\[?Yield\]](#TemplateLiteral)  

### <a name="Arguments"> Arguments\[Yield]  
- ___\(___ ___)___  
- ___\(___ [ArgumentList\[?Yield\]](#ArgumentList) ___)___  

### <a name="ArgumentList"> ArgumentList\[Yield]  
- [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression)  
- ___...___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression)  
- [ArgumentList\[?Yield\]](#ARgumentList) ___,___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression)  
- [ArgumentList\[?Yield\]](#ARgumentList) ___,___ ___...___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression)  

### <a name="LeftHandSideExpression"> LeftHandSideExpression\[Yield]  
- [NewExpression\[?Yield\]](#NewExpression)  
- [CallExpression\[?Yield\]](#CallExpression)  

### <a name="PrimaryExpression"> PrimaryExpression\[Yield]  
- ___this___  
- [IdentifierReference\[?Yield\]](#IdentifierReference)  
- [Literal](#Literal)  
- [ArrayInitializer\[?Yield\]](#ArrayInitializer)  
- [ObjectLiteral\[?Yield\]](#ObjectLiteral)  
- [FunctionExpression](#FunctionExpression)  
- [ClassExpression](#ClassExpression)  
- [GeneratorExpression](#GeneratorExpression)  
- [GeneratorComprehension\[?Yield\]](#GeneratorComprehension)  
- [RegularExpressionLiteral](#RegularExpressionLiteral)  
- [TemplateLiteral\[?Yield\]](#TemplateLiteral)  
- [CoverParenthesizedExpressionAndArrowParameterList\[?Yield\]](#CoverParenthesizedExpressionAndArrowParameterList)  

### <a name="CoverParenthesizedExpressionAndArrowParameterList"> CoverParenthesizedExpressionAndArrowParameterList\[Yield]  
- ___\(___ [Expression\[In, ?Yield\]](#Expression) ___)___  
- ___\(___ ___)___    
- ___\(___ ___...___ [BindingIdentifier\[?Yield\]](#BindingIdentifier) )  
- ___\(___ [Expression\[In, ?Yield\]](#Expression) ___,___ ___...___ [BindingIdentifier\[?Yield\]](#BindingIdentifier) )  

### <a name="ArrayLiteral"> ArrayLiteral\[Yield]  
- ___\[___ [Elision\(opt\)](#Elision) ___]___  
- ___\[___ [ElementList\[?Yield\]](#ElementList) ___]___  
- ___\[___ [ElementList\[?Yield\]](#ElementList) , [Elision\(opt\)](#Elision) ___]___  

### <a name="ElementList"> ElementList\[Yield]  
- [Elision\(opt\)](#Elision) [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression)  
- [Elision\(opt\)](#Elision) [SpreadElement\[?Yield\]](#SpreadElement)  
- [ElementList\[?Yield\]](#ElementList) ___,___ [Elision\(opt\)](#Elision) [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression)
- [ElementList\[?Yield\]](#ElementList) ___,___ [Elision\(opt\)](#Elision) [SpreadElement\[?Yield\]](#SpreadElement)  

### <a name="Elision"> Elision  
- ___,___  
- [Elision](#Elision) ___,___  

### <a name="SpreadElement"> SpreadElement\[Yield]  
- ___...___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression)  

### <a name="ArrayComprehension"> ArrayComprehension\[Yield]  
- ___\[___ [Comprehension\[?Yield\]](#Comprehension) ___]___  

### <a name="Comprehension"> Comprehension\[Yield]  
- [ComprehensionFor\[?Yield\]](#ComprehensionFor) [ComprehensionTail\[?Yield\]](#ComprehensionTail)  

### <a name="ComprehensionTail"> ComprehensionTail\[Yield]  
- [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression) [ComprehensionFor\[?Yield\]](#ComprehensionFor) [ComprehensionTail\[?Yield\]](#ComprehensionTail) [ComprehensionIf\[?Yield\]](#ComprehensionIf) [ComprehensionTail\[?Yield\]](#ComprehensionTail)  

### <a name="ComprehensionFor"> ComprehensionFor\[Yield]  
- ___for___ ___\(___ [ForBinding\[?Yield\]](#ForBinding) ___of___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression) ___)___  

### <a name="ComprehensionIf"> ComprehensionIf\[Yield]  
- ___if___ ___\(___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression) ___)___  

### <a name="ForBinding"> ForBinding\[Yield]  
- [BindingIdentifier\[?Yield\]](#BindingIdentifier)  
- [BindingPattern\[?Yield\]](#BindingPattern)  

### <a name="ObjectLiteral"> ObjectLiteral\[Yield]  
- ___{___ ___}___  
- ___{___ [PropertyDefinitionList\[?Yield\]](#PropertyDefinitionList) ___}___  
- ___{___ [PropertyDefinitionList\[?Yield\]](#PropertyDefinitionList) ___,___ ___}___  

### <a name="PropertyDefinitionList"> PropertyDefinitionList\[YieldYield]  
- [PropertyDefinition\[?YieldYield\]](#PropertyDefinition)  
- [PropertyDefinitionList\[?YieldYield\]](#PropertyDefinitionList) ___,___ [PropertyDefinition\[?YieldYield\]](#PropertyDefinition)  

### <a name="PropertyDefinition"> PropertyDefinition\[Yield]  
- [IdentifierReference\[?Yield\]](#IdentifierReference)  
- [CoverInitializedName\[?Yield\]](#CoverInitializedName)  
- [PropertyName\[?Yield\]](#PropertyName) ___:___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression)  
- [MethodDefinition\[?Yield\]](#MethodDefinition)  

### <a name="PropertyName"> PropertyName\[Yield,GeneratorParameter]  
- [LiteralPropertyName](#LiteralPropertyName)  
- \[+GeneratorParameter] [ComputedPropertyName](#ComputedPropertyName)  
- \[~GeneratorParameter] [ComputedPropertyName\[?Yield\]](#ComputedPropertyName)  

### <a name="LiteralPropertyName"> LiteralPropertyName  
- [IdentifierName](#IdentifierName)  
- [StringLiteral](#StringLiteral)  
- [NumericLiteral](#NumericLiteral)  

### <a name="ComputedPropertyName"> ComputedPropertyName\[Yield]  
- ___\[___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression) ___]___  

### <a name="CoverInitializedName"> CoverInitializedName\[Yield]  
- [IdentifierReference\[?Yield\]](#IdentifierReference) [Initializer\[In, ?Yield\]](#Initializer)  

### <a name="Initializer"> Initializer\[In, Yield]  
- ___=___ [AssignmentExpression\[?In, ?Yield\]](#AssignmentExpression)  

### <a name="Literal"> Literal  
- [NullLiteral](#NullLiteral)  
- [ValueLiteral](#ValueLiteral)  

### <a name="ValueLiteral"> ValueLiteral  
- [BooleanLiteral](#BooleanLiteral)  
- [NumericLiteral](#NumericLiteral) [StringLiteral](#StringLiteral)  

### <a name="IdentifierReference"> IdentifierReference\[Yield]  
- [Identifier](#Identifier)  
- \[~Yield] ___yield___  

### <a name="BindingIdentifier"> BindingIdentifier\[Default, Yield]  
- \[+Default] ___default___  
- \[~Yield] ___yield___ [Identifier](#Identifier)  

### <a name="LabelIdentifier"> LabelIdentifier\[Yield]  
- [Identifier](#Identifier)  
- \[~Yield] ___yield___  

### <a name="Identifier"> Identifier  
- [IdentifierName](#IdentifierName) __but not ReservedWord__  
