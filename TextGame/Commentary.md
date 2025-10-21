# COMMENTARY.md


### Data Structure Used

The story is built using a small **graph** data structure.  
Each node of the graph is represented by a `Scene` object containing:

- `title` – a readable heading for the scene.  
- `body` – a `vector<string>` storing multiple lines of narrative text.  
- `options` – a `vector<Option>` where each `Option` stores a player choice and the index of the next connected scene.  
- `isEnding` – marks terminal nodes with no further choices.  
- `endingTag` – labels the type of ending, such as *UNLOCK*, *CAUGHT*, or *ESCAPE*.  

All scenes are stored in a single `vector<Scene>`, where the index acts as the node ID.  

---

### Player Choices and Navigation

Player input is handled through numbered options printed for each scene.  
The `getChoice()` function validates the input and ensures only valid numeric entries are accepted.  
Each player choice corresponds to an `Option` entry, and the game uses `nextId` to jump directly to the next scene, maintaining a clear and simple navigation structure.

A `State` struct tracks the player’s inventory and condition, influencing branching decisions:

- `hasKey` – found in the Cell; required to unlock the final gate (good ending).  
- `foundHook` – found in the Armoury; allows safe retrieval of the chest’s contents if the player doesn’t have the key.  
- `injured` – set if the player reaches into the trapped chest without either item.  

If the player already has the key, searching the racks later provides a logical response (“You already have what you need”) instead of redundant items.  
If the player reaches into the chest without a key or hook, they become injured and are automatically sent back to the Cell scene to recover.  


### Challenges

The main challenge was ensuring the **narrative flow remained logical** while still being driven by a simple graph structure.  


