# Expression  
### <a name="Expression"> Expression\[In, Yield]  
- [AssignmentExpression](#AssignmentExpression)\[?In, ?Yield]  
- [Expression](#Expression)\[?In, ?Yield] ___,___ [AssignmentExpression](#AssignmentExpression)\[?In, ?Yield]  

### <a name="AssignmentPattern"> AssignmentPattern\[Yield]  
- [ObjectAssignmentPattern](#ObjectAssignmentPattern)\[?Yield]  
- [ArrayAssignmentPattern](#ArrayAssignmentPattern)\[?Yield]  

### <a name="ObjectAssignmentPattern"> ObjectAssignmentPattern\[Yield]  
- ___{___ ___}___  
- ___{___ [AssignmentPropertyList](#AssignmentPropertyList)\[?Yield] ___}___  
- ___{___ [AssignmentPropertyList](#AssignmentPropertyList)\[?Yield] , ___}___  

### <a name="ArrayAssignmentPattern"> ArrayAssignmentPattern\[Yield]  
- ___\[___ [Elision](#Elision)\(opt) [AssignmentRestElement](#AssignmentRestElement)\[?Yield]\(opt) ___]___  
- ___\[___ [AssignmentElementList](#AssignmentElementList)\[?Yield] ___]___  
- ___\[___ [AssignmentElementList](#AssignmentElementList)\[?Yield] , [Elision](#Elision)\(opt) [AssignmentRestElement](#AssignmentRestElement)\[?Yield]\(opt) ___]___  

### <a name="AssignmentPropertyList"> AssignmentPropertyList\[Yield]  
- [AssignmentProperty](#AssignmentProperty)\[?Yield]  
- [AssignmentPropertyList](#AssignmentPropertyList)\[?Yield] ___,___ AssignmentProperty\[?Yield]  

### <a name="AssignmentElementList"> AssignmentElementList\[Yield]  
- [AssignmentElisionElement](#AssignmentElisionElement)\[?Yield]  
- [AssignmentElementList](#AssignmentElementList)\[?Yield] ___,___ [AssignmentElisionElement](#AssignmentElisionElement)\[?Yield]  

### <a name="AssignmentElisionElement"> AssignmentElisionElement\[Yield]  
- [Elision](#Elision)\(opt) [AssignmentElement](#AssignmentElement)\[?Yield]  

### <a name="AssignmentProperty"> AssignmentProperty\[Yield] :  
- [IdentifierReference](#IdentifierReference)\[?Yield] [Initializer](#Initializer)\[In,?Yield]\(opt)  
- [PropertyName](#PropertyName) ___:___ [AssignmentElement](#AssignmentElement)\[?Yield]  

### <a name="AssignmentElement"> AssignmentElement\[Yield]  
- [DestructuringAssignmentTarget](#DestructuringAssignmentTarget)\[?Yield] [Initializer](#Initializer)\[In,?Yield]\(opt)  

### <a name="AssignmentRestElement"> AssignmentRestElement\[Yield]  
- ___...___ [DestructuringAssignmentTarget](#DestructuringAssignmentTarget)\[?Yield]  

### <a name="DestructuringAssignmentTarget"> DestructuringAssignmentTarget\[Yield]  
- [LeftHandSideExpression](#LeftHandSideExpression)\[?Yield]  

### <a name="AssignmentExpression"> AssignmentExpression\[In, Yield]  
- [ConditionalExpression](#ConditionalExpression)\[?In, ?Yield]  
- \[+Yield] [YieldExpression](#YieldExpression)\[?In]  
- [ArrowFunction](#ArrowFunction)\[?In, ?Yield]  
- [LeftHandSideExpression](#LeftHandSideExpression)\[?Yield] ___=___ [AssignmentExpression](#AssignmentExpression)\[?In,?Yield]  
- [LeftHandSideExpression](#LeftHandSideExpression)\[?Yield] [AssignmentOperator](#AssignmentOperator) [AssignmentExpression](#AssignmentExpression)\[?In, ?Yield]  

### <a name="AssignmentOperator"> AssignmentOperator  
- *one of* `*= /= %= += -= <<= >>= >>>= &= ^= |=`  

### <a name="ConditionalExpression"> ConditionalExpression\[In, Yield]  
- [LogicalORExpression](#LogicalORExpression)\[?In, ?Yield]  
- [LogicalORExpression](#LogicalORExpression)\[?In,?Yield] ___?___ [AssignmentExpression](#AssignmentExpression)\[In, ?Yield] ___:___ [AssignmentExpression](#AssignmentExpression)\[?In, ?Yield]  

### <a name="BitwiseORExpression"> BitwiseORExpression\[In, Yield]  
- [BitwiseXORExpression](#BitwiseXORExpression)\[?In, ?Yield]  
- [BitwiseORExpression](#BitwiseORExpression)\[?In, ?Yield] ___|___ [BitwiseXORExpression](#BitwiseXORExpression)\[?In, ?Yield]  

### <a name="BitwiseXORExpression"> BitwiseXORExpression\[In, Yield]  
- [BitwiseANDExpression](#BitwiseANDExpression)\[?In, ?Yield]  
- [BitwiseXORExpression](#BitwiseXORExpression)\[?In, ?Yield] ___^___ [BitwiseANDExpression](#BitwiseANDExpression)\[?In, ?Yield]  

### <a name="BitwiseANDExpression"> BitwiseANDExpression\[In, Yield]  
- [EqualityExpression](#EqualityExpression)\[?In, ?Yield]  
- [BitwiseANDExpression](#BitwiseANDExpression)\[?In, ?Yield] ___&___ [EqualityExpression](#EqualityExpression)\[?In, ?Yield]  

### <a name="EqualityExpression"> EqualityExpression\[In, Yield]  
- [RelationalExpression](#RelationalExpression)\[?In, ?Yield]  
- [EqualityExpression](#EqualityExpression)\[?In, ?Yield] ___==___ [RelationalExpression](#RelationalExpression)\[?In, ?Yield]  
- [EqualityExpression](#EqualityExpression)\[?In, ?Yield] ___!=___ [RelationalExpression](#RelationalExpression)\[?In, ?Yield]  
- [EqualityExpression](#EqualityExpression)\[?In, ?Yield] ___===___[RelationalExpression](#RelationalExpression)\[?In, ?Yield]  
- [EqualityExpression](#EqualityExpression)\[?In, ?Yield] ___!==___[RelationalExpression](#RelationalExpression)\[?In, ?Yield]  

### <a name="RelationalExpression"> RelationalExpression\[In, Yield]  
- [ShiftExpression](#ShiftExpression)\[?Yield]  
- [RelationalExpression](#RelationalExpression)\[?In, ?Yield] ___<___ [ShiftExpression](#ShiftExpression)\[?Yield]  
- [RelationalExpression](#RelationalExpression)\[?In, ?Yield] ___>___ [ShiftExpression](#ShiftExpression)\[?Yield]  
- [RelationalExpression](#RelationalExpression)\[?In, ?Yield] ___<=___ [ShiftExpression](#ShiftExpression)\[? Yield]  
- [RelationalExpression](#RelationalExpression)\[?In, ?Yield] ___>=___ [ShiftExpression](#ShiftExpression)\[?Yield]  
- [RelationalExpression](#RelationalExpression)\[?In, ?Yield] ___instanceof___ [ShiftExpression](#ShiftExpression)\[?Yield]  
- \[+In] [RelationalExpression](#RelationalExpression)\[In, ?Yield] ___in___ [ShiftExpression](#ShiftExpression)\[?Yield]  

### <a name="ShiftExpression"> ShiftExpression\[Yield]  
- [AdditiveExpression](#AdditiveExpression)\[?Yield]  
- [ShiftExpression](#ShiftExpression)\[?Yield] ___<<___ [AdditiveExpression](#AdditiveExpression)\[?Yield]  
- [ShiftExpression](#ShiftExpression)\[?Yield] ___>>___ [AdditiveExpression](#AdditiveExpression)\[?Yield]  
- [ShiftExpression](#ShiftExpression)\[?Yield] ___>>>___ [AdditiveExpression](#AdditiveExpression)\[?Yield]  

### <a name="AdditiveExpression"> AdditiveExpression\[Yield]  
- [MultiplicativeExpression](#MultiplicativeExpression)\[?Yield]  
- [AdditiveExpression](#AdditiveExpression)\[?Yield] ___+___ [MultiplicativeExpression](#MultiplicativeExpression)\[?Yield]  
- [AdditiveExpression](#AdditiveExpression)\[?Yield] ___-___ [MultiplicativeExpression](#MultiplicativeExpression)\[?Yield]  

### <a name="MultiplicativeExpression"> MultiplicativeExpression\[Yield]  
- [UnaryExpression](#UnaryExpression)\[?Yield]  
- [MultiplicativeExpression](#MultiplicativeExpression)\[?Yield] ___*___ [UnaryExpression](#UnaryExpression)\[?Yield]  
- [MultiplicativeExpression](#MultiplicativeExpression)\[?Yield] ___/___ [UnaryExpression](#UnaryExpression)\[?Yield]  
- [MultiplicativeExpression](#MultiplicativeExpression)\[?Yield] ___%___ [UnaryExpression](#UnaryExpression)\[?Yield]  

### <a name="UnaryExpression"> UnaryExpression\[Yield]  
- [PostfixExpression](#PostfixExpression)\[?Yield]  
- ___delete___ [UnaryExpression](#UnaryExpression)\[?Yield]  
- ___void___ [UnaryExpression](#UnaryExpression)\[?Yield]  
- ___typeof___ [UnaryExpression](#UnaryExpression)\[?Yield]  
- ___++___ [UnaryExpression](#UnaryExpression)\[?Yield]  
- ___--___ [UnaryExpression](#UnaryExpression)\[?Yield]  
- ___+___ [UnaryExpression](#UnaryExpression)\[?Yield]  
- ___-___ [UnaryExpression](#UnaryExpression)\[?Yield]  
- ___~___ [UnaryExpression](#UnaryExpression)\[?Yield]  
- ___!___ [UnaryExpression](#UnaryExpression)\[?Yield]  

### <a name="PostfixExpression"> PostfixExpression\[Yield]  
- LeftHandSideExpression\[?Yield]  
- LeftHandSideExpression\[?Yield] \[no LineTerminator here] ___++___  
- LeftHandSideExpression\[?Yield] \[no LineTerminator here] ___--___  

### <a name="MemberExpression"> MemberExpression\[Yield]  
- \[Lexical goal InputElementRegExp] PrimaryExpression\[?Yield]  
- MemberExpression\[?Yield] ___\[___ Expression\[In, ?Yield] ___]___  
- MemberExpression\[?Yield] ___.___ IdentifierName  
- MemberExpression\[?Yield] TemplateLiteral\[?Yield]  
- ___super___ \[ Expression\[In, ?Yield] ]  
- ___super___ ___.___ IdentifierName  
- ___new___ ___super___ Arguments\[?Yield]\(opt)  
- ___new___ ___\[___ *lookahead*  ___{___ ___super___ ___}___ ___]___ MemberExpression\[?Yield]  

### <a name="NewExpression"> NewExpression\[Yield]  
- MemberExpression\[?Yield]  
- ___new___ NewExpression\[?Yield]  
- ___new___ ___super___  

### <a name="CallExpression"> CallExpression\[Yield]  
- MemberExpression\[?Yield] Arguments\[?Yield]  
- ___super___ Arguments\[?Yield]  
- CallExpression\[?Yield] Arguments\[?Yield]  
- CallExpression\[?Yield] ___\[___ Expression\[In, ?Yield] ___]___  
- CallExpression\[?Yield] ___.___ IdentifierName  
- CallExpression\[?Yield] TemplateLiteral\[?Yield]  

### <a name="Arguments"> Arguments\[Yield]  
- ___\(___ ___)___  
- ___\(___ ArgumentList\[?Yield] ___)___  

### <a name="ArgumentList"> ArgumentList\[Yield]  
- AssignmentExpression\[In, ?Yield]  
- ___...___ AssignmentExpression\[In, ?Yield]  
- ArgumentList\[?Yield] ___,___ AssignmentExpression\[In, ?Yield]  
- ArgumentList\[?Yield] ___,___ ___...___ AssignmentExpression\[In, ?Yield]  

### <a name="LeftHandSideExpression"> LeftHandSideExpression\[Yield]  
- NewExpression\[?Yield]  
- CallExpression\[?Yield]  

### <a name="PrimaryExpression"> PrimaryExpression\[Yield]  
- ___this___  
- IdentifierReference\[?Yield]  
- Literal  
- ArrayInitializer\[?Yield]  
- ObjectLiteral\[?Yield]  
- FunctionExpression  
- ClassExpression  
- GeneratorExpression  
- GeneratorComprehension\[?Yield]  
- RegularExpressionLiteral  
- TemplateLiteral\[?Yield]  
- CoverParenthesizedExpressionAndArrowParameterList\[?Yield]  

### <a name="CoverParenthesizedExpressionAndArrowParameterList"> CoverParenthesizedExpressionAndArrowParameterList\[Yield]  
- ___\(___ Expression\[In, ?Yield] ___)___  
- ___\(___ ___)___    
- ___\(___ ___...___ BindingIdentifier\[?Yield] )  
- ___\(___ Expression\[In, ?Yield] ___,___ ___...___ BindingIdentifier\[?Yield] )  

### <a name="ArrayLiteral"> ArrayLiteral\[Yield]  
- ___\[___ Elision\(opt) ___]___  
- ___\[___ ElementList\[?Yield] ___]___  
- ___\[___ ElementList\[?Yield] , Elision\(opt) ___]___  

### <a name="ElementList"> ElementList\[Yield]  
- Elision\(opt) AssignmentExpression\[In, ?Yield]  
- Elision\(opt) SpreadElement\[?Yield]  
- ElementList\[?Yield] ___,___ Elision\(opt) AssignmentExpression\[In, ?Yield]
- ElementList\[?Yield] ___,___ Elision\(opt) SpreadElement\[?Yield]  

### <a name="Elision"> Elision  
- ___,___  
- Elision ___,___  

### <a name="SpreadElement"> SpreadElement\[Yield]  
- ___...___ AssignmentExpression\[In, ?Yield]  

### <a name="ArrayComprehension"> ArrayComprehension\[Yield]  
- ___\[___ Comprehension\[?Yield] ___]___  

### <a name="Comprehension"> Comprehension\[Yield]  
- ComprehensionFor\[?Yield] ComprehensionTail\[?Yield]  

### <a name="ComprehensionTail"> ComprehensionTail\[Yield]  
- AssignmentExpression\[In, ?Yield] ComprehensionFor\[?Yield] ComprehensionTail\[?Yield] ComprehensionIf\[?Yield] ComprehensionTail\[?Yield]  

### <a name="ComprehensionFor"> ComprehensionFor\[Yield]  
- for ___\(___ ForBinding\[?Yield] ___of___ AssignmentExpression\[In, ?Yield] ___)___  

### <a name="ComprehensionIf"> ComprehensionIf\[Yield]  
- ___if___ ___\(___ AssignmentExpression\[In, ?Yield] ___)___  

### <a name="ForBinding"> ForBinding\[Yield]  
- BindingIdentifier\[?Yield]  
- BindingPattern\[?Yield]  

### <a name="ObjectLiteral"> ObjectLiteral\[Yield]  
- ___{___ ___}___  
- ___{___ PropertyDefinitionList\[?Yield] ___}___  
- ___{___ PropertyDefinitionList\[?Yield] ___,___ ___}___  

### <a name="PropertyDefinitionList"> PropertyDefinitionList\[YieldYield]  
- PropertyDefinition\[?YieldYield]  
- PropertyDefinitionList\[?YieldYield] ___,___ PropertyDefinition\[?YieldYield]  

### <a name="PropertyDefinition"> PropertyDefinition\[Yield]  
- IdentifierReference\[?Yield]  
- CoverInitializedName\[?Yield]  
- PropertyName\[?Yield] ___:___ AssignmentExpression\[In, ?Yield]  
- MethodDefinition\[?Yield]  

### <a name="PropertyName"> PropertyName\[Yield,GeneratorParameter]  
- LiteralPropertyName  
- \[+GeneratorParameter] ComputedPropertyName  
- \[~GeneratorParameter] ComputedPropertyName\[?Yield]  

### <a name="LiteralPropertyName"> LiteralPropertyName  
- IdentifierName  
- StringLiteral  
- NumericLiteral  

### <a name="ComputedPropertyName"> ComputedPropertyName\[Yield]  
- ___\[___ AssignmentExpression\[In, ?Yield] ___]___  

### <a name="CoverInitializedName"> CoverInitializedName\[Yield]  
- IdentifierReference\[?Yield] Initializer\[In, ?Yield]  

### <a name="Initializer"> Initializer\[In, Yield]  
- = AssignmentExpression\[?In, ?Yield]  

### <a name="Literal"> Literal  
- NullLiteral  
- ValueLiteral  

### <a name="ValueLiteral"> ValueLiteral  
- BooleanLiteral  
- NumericLiteral StringLiteral  

### <a name="IdentifierReference"> IdentifierReference\[Yield]  
- Identifier  
- \[~Yield] ___yield___  

### <a name="BindingIdentifier"> BindingIdentifier\[Default, Yield]  
- \[+Default] ___default___  
- \[~Yield] ___yield___ Identifier  

### <a name="LabelIdentifier"> LabelIdentifier\[Yield]  
- Identifier  
- \[~Yield] ___yield___  

### <a name="Identifier"> Identifier  
- IdentifierName *but not ReservedWord*  
