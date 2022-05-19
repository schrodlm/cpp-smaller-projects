## Zadani

Úkolem je realizovat třídu, která bude reprezentovat datum. Reprezentovaná data budou odpovídat pravidlům Gregoriánského kalendáře, je požadováno, aby šlo reprezentovat data nejméně v rozmezí let 2000 až 2030.

Třída CDate musí splňovat následující rozhraní:

-   konstruktor s parametry (y,m,d) vytvoří novou instanci třídy s datem nastaveným podle předaných hodnot. Konstruktor musí kontrolovat, zda zadané datum je platné. Pokud platné není, musí vyvolat výjimku InvalidDateException,
-   operátorem + lze k instanci CDate přičíst celé číslo a tím se posunout vpřed o zadaný počet dní (vzad pro záporné číslo),
-   operátorem - lze od instance CDate odečíst celé číslo a tím se posunout vzad o zadaný počet dní (vpřed pro záporné číslo),
-   operátorem - lze od sebe odečíst dvě instance CDate a tím získat počet dní mezi těmito dvěma daty,
-   operátory ++ a -- v prefixové a v postfixové notaci lze zvyšovat/snižovat datum o 1 den, operátory mají obvyklé chování,
-   operátory ==, !=, >, >=, < a <= lze porovnávat dvojici instancí CDate, v této relaci budou data v budoucnosti větší než data v minulosti,
-   operátorem << lze zobrazit instanci CDate v zadaném streamu. Při zobrazování se používá ISO formát (%Y-%m-%d, tedy např. 2000-01-31). V povinných testech bude vždy použit tento implicitní formát. Bonusové testy požadují implementaci manipulátoru date\_format, kterým lze formát řídit.
-   operátorem >> lze přečíst instanci CDate ze zadaného streamu. V povinných testech je na vstupu očekáváno datum v ISO formátu %Y-%m-%d. Pokud se nepodaří datum načíst (formát, neplatné datum, ...), operátor zajistí nastavení fail bitu a ponechá původní obsah instance CDate. Stejně jako výstupní operátor, i vstup lze řídit pomocí manipulátoru date\_format, tato možnost je požadovaná v bonusovém testu.

