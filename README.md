# json format

below is an interval map labeled "just intonation" with specified note maps. intervals can be either ratio or cent values. basemap must contain all 12 intervals. hold shift to reload.

```json
{
  "basemap": {
    "name": "just intonation",
    "intervals": {
      "0": "1:1",
      "1": "16:15",
      "2": "9:8",
      "3": "6:5",
      "4": "5:4",
      "5": "4:3",
      "6": "64:45",
      "7": "3:2",
      "8": "8:5",
      "9": "5:3",
      "10": "9:5",
      "11": "15:8"
    }
  },
  
  "notemaps": {
    "4": {
      "name": "undecimal neutral third",
      "intervals": {
        "3": "11:9",
        "4": "11:9"
      }
    },

    "3": {
      "name": "subminor third",
      "intervals": {
        "3": "7:6",
        "4": "7:6"
      }
    },
    
    "5": {
      "name": "septimal major third",
      "intervals": {
        "3": "9:7",
        "4": "9:7"
      }
    }
  }
}
```

# how to select note map

1. press and hold down root note.
2. while root note is held, press the corresponding interval as specified in the note map (e.g., to select note map listed "5", press 5 semitones from the root, so in the case of C, press F.)
3. press the same interval again to toggle off.

*note*: you can select whether or not the note map resets on root change, see *toggle note map* in settings.

input range is reserved for root note changes. you can enable *pedal* to create a drone.
