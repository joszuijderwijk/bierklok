# Bierklok
Een fysieke klok met een BKA30D-R5 stappenmotor die de huidige bieraanbieding weergeeft. De motor wordt aangestuurd met een ESP8266 module via MQTT. In mijn setup luistert de klok naar data van [kratjes.net](https://kratjes.net). Meer informatie over dit project lees je in [deze blogpost](https://joszuijderwijk.nl/blog/bierklok/).

[![bierklok](https://img.youtube.com/vi/E2udo9U9OrQ/0.jpg)](https://www.youtube.com/watch?v=E2udo9U9OrQ)

## Rainmeter
Deze repo bevat ook de rainmeterskin voor [https://kratjes.net](https://kratjes.net)

### Variatie 1
![Klok](https://i.imgur.com/Amryve0.gif)

Een klok die de bonusaanbiedingen per supermarkt aangeeft. Klik op de supermarktdisplay om naar de volgende supermarkt te gaan. Als een supermarkt geen aanbiedingen heeft, gaat de Skin automatisch naar de volgende. De klok geeft maximaal twee aanbiedingen weer ([meer informatie](https://joszuijderwijk.nl/blog/kratjes)).

### Variatie 2
![dopjes](https://i.imgur.com/vGaZBOo.png)

De dopjes van de bonusaanbiedingen. Geeft maximaal vier aanbiedingen weer. De gewenste supermarkt kan eenvoudig aangepast worden in _dopjes.ini_.

### Installatie

1. Installeer [Rainmeter](https://www.rainmeter.net/)
2. Download en installleer de [.rmskin package](https://github.com/iovidius/bierklok/raw/main/Rainmeter/Bierklok_1.3.rmskin)

Of kopieer de bestanden uit de _bierklok_ map direct naar je _Skins_ map en refresh Rainmeter.
