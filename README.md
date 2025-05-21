# Tsubasamusu Unreal Assist
## Test Engine Versions
- 5.5.4
# Features
## Tools in Blueprint Graph
### Make Array
When multiple nodes with output pins of the same type are selected and right-clicked, the “Make Array” menu appears, and selecting the menu creates a Make Array node with all of those output pins connected.

<img width="500" src="https://github.com/user-attachments/assets/1b7cf903-4eba-44a6-b475-5e513eb4b4d5">

### Translate Comment Node
When you right-click on a comment node, a menu labeled 'Translate to...' appears, allowing you to select the target language and translate the comment. This translation feature uses the DeepL API, and the API key can be configured in the editor settings under Plugins > Tsubasamusu Unreal Assist > Comment Node > DeepL API Key.

<img width="500" src="https://github.com/user-attachments/assets/6768ed5d-5c88-42e7-b2f4-e33a4e4b24c8">

## Blueprint Function Library for Editor Utilty
### Replace Assets 

References

<img width="500" src="https://github.com/user-attachments/assets/1435b874-ef23-45e7-8ee6-03ce2aeaf54a">

### Create a Material Instance Asset
Create a Material Instance asset in the content drawer. ``InAssetDirectory`` should be set to a string such as "/Game/FolderName1/FolderName2/".

<img width="500" src="https://github.com/user-attachments/assets/dbfd43c9-612e-4861-b161-4d05109fddf1">
