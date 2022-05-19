## Zadání

Úkolem je realizovat sadu tříd, které budou implementovat popis některých datových typů v C/C++.

Kompilátor si během kompilace musí pamatovat vlastnosti datových typů. Některé datové typy jsou v kompilátoru pevně zabudované (např. int, double), jiné si programátor může definovat ve svém programu (odvozené typy: např. enum, struct). Sada implementovaných tříd dokáže zachytit popis takových typů. Konkrétně chceme popisovat pouze některé typy (int, double, enum a struct), navíc pro ně chceme pouze některé operace.

Očekává se, že implementujete následující třídy s následujícím rozhraním:

## CDataTypeInt

Třída reprezentuje primitivní typ int. Má následující rozhraní:

__implicitní konstruktor__

inicializuje instanci třídy,

getSize()

metoda vrátí velikost typu, zde vždy 4,

__operátor ==__

porovná tento typ s jiným typem, vrátí true, pokud jsou oba typy shodné (oba jsou typy int),

__operátor !=__

porovná tento typ s jiným typem, vrátí true, pokud jsou typy rozdílné,

__operátor <<__

zobrazí název typu do zadaného proudu.

## CDataTypeDouble

Třída reprezentuje primitivní typ double. Má následující rozhraní:

__implicitní konstruktor__

inicializuje instanci třídy,

__getSize()__

metoda vrátí velikost typu, zde vždy 8,

__operátor ==__

porovná tento typ s jiným typem, vrátí true, pokud jsou oba typy shodné (oba jsou typy double),

__operátor !=__

porovná tento typ s jiným typem, vrátí true, pokud jsou typy rozdílné,

__operátor <<__

zobrazí název typu do zadaného proudu.

## CDataTypeEnum

Třída reprezentuje výčtový typ. Má následující rozhraní:

__implicitní konstruktor__

inicializuje instanci třídy,

__getSize()__

metoda vrátí velikost typu, zde vždy 4,

__add()__

metoda přidá další hodnotu do výčtu. Pokud stejná hodnota ve výčtu již existuje, je vyvolaná výjimka (viz ukázkový běh),

__operátor ==__

porovná tento typ s jiným typem, vrátí true, pokud jsou oba typy shodné (oba jsou výčtové typy a mají stejný výčet hodnot ve stejném pořadí),

__operátor !=__

porovná tento typ s jiným typem, vrátí true, pokud jsou typy rozdílné,

__operátor <<__

zobrazí název typu do zadaného proudu. Pozor, hodnoty výčtu musí být zobrazené v pořadí zadávání.

## CDataTypeStruct

Třída reprezentuje typ struktura. Má následující rozhraní:

__implicitní konstruktor__

inicializuje instanci třídy,

__getSize()__

metoda vrátí velikost typu (dle obsažených složek),

__addField(name, type)__

metoda přidá další složku zadaného jména a typu (int/double/enum) na konec seznamu složek. Pokud je jméno složky duplicitní, vyhlásí výjimku (viz ukázkový běh),

__field(name)__

zpřístupní složku zadaného jména, případně vyhodí výjimku pokud složka takového jména neexistuje (viz ukázkový běh). Složka zpřístupněná touto metodou bude pouze čtena,

__operátor ==__

porovná tento typ s jiným typem, vrátí true, pokud jsou oba typy shodné (oba typy jsou struktury, mají stejný počet složek, složky na každé pozici mají stejný typ, jména složek se ale mohou lišit),

__operátor !=__

porovná tento typ s jiným typem, vrátí true, pokud jsou typy rozdílné,

__operátor <<__

zobrazí název typu do zadaného proudu. Pořadí složek odpovídá pořadí jejich přidání metodou addField.

