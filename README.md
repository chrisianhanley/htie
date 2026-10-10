supports MTS-ESP

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
    "3": {
      "name": "subminor third",
      "intervals": {
        "3": "7:6",
        "4": "7:6"
      }
    },

    "4": {
      "name": "undecimal neutral third",
      "intervals": {
        "3": "11:9",
        "4": "11:9"
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

# terminology

i use some of my own theory in conceptualizing what this plugin is doing as it helps me visualize things. feel free to read below...

**intro to functional harmony in non-equal tuning systems**  
important terms!!!  
by general order of significance  
**two-dimensional scale (or multi-dimensional scale)**: a scale with separate subscales built off each scale degree. a two-dimensional scale can be formed from any non-equal temperament, as you can build the scale starting from C, and then build another scale starting from each scale degree down the list. a two-dimensional scale can be broken down to form one long linear scale, which can form a temperament — a kind of hybrid temperament. These hybrid temperaments, or multi-dimensional temperaments to be specific, are essential for ensuring consistent harmonies across a linear tuning system (granted that it’s a non-equal temperament), as it allows for each harmony relative to the tonic to be created for each scale degree down the list, for example, C to its just major third, E is created for F to its just major third, A, and now, with these pitches available to us, we can play a C just major chord (see just chords) while simultaneously being able to play an F just major chord all within the same tuning system.  
**three-dimensional scale** - the concept of building scales from a two-dimensional scale based on a third variable or argument (also known as note mapping, see hybrid temperament interval engine): for example, if z = 1, then i’ll replace all minor thirds with their sub minor equivalents, if z = 2, then i’ll replace all major thirds with their supermajor equivalents, etc.
	- assumes the existence of sub-temperaments  
**subscale** - a scale within a scale as defined above.  
**scale shifting** - the concept of “selecting” a group of pitches among different scales or subscales as a compositional technique (this is essentially what my plugin does, called base mapping).  
**temperamental interchange** - the concept of borrowing intervals from other tuning systems (as it relates to modal interchange).  
**hybrid temperament** - a tuning system comprised of multiple other tuning systems or scales.  
**unison progression** - the concept of using identical ratios as a melody.  
**identical ratios** - (in a microtonal context) ratios pointing to the same scale degree, similar to enharmonic equivalents pointing to the same pitch, for example, a just major third and a septimal major third would both point to E when starting from C.  
**linear tuning system** - any tuning system comprised of a linear scale (so basically every tuning system before this).  
	- conversely, a non-linear tuning system would be comprised of a non-linear scale, and by extension, a two-dimensional scale would be a type of non-linear scale.  
**imperfect intervals** - referring to “imperfect” versions of otherwise perfect intervals, such as the “wolf fifth”, implying the existence of the imperfect fourth.  

**examples of functional harmony in non-equal tuning systems**  
https://www.31edo.com/tricesimoprimal  
**approximate tuning** - describes a tuning system that is an approximation of a larger one, usually by taking intervals that most closely align.  
**example of a 12 tone approximate tuning of 31edo (12tat or 12tat31):**  
	1  
	116.13  
	2  
	193.55  
	3  
	309.68  
	4  
	387.1  
	5  
	503.23  
	6  
	580.65  
	7  
	696.77  
	8  
	812.9  
	9  
	890.32  
	10  
	1006.45  
	11  
	1083.87  

**quarter-tone approximation** - a 24 note approximation of a larger tuning system (e.g., 24tat, 24tat31)
alternate chord centers - referring to chord centers that exist outside the standard 12 tone system.
*Edit June 16th, 2026:* notation can also include limit tunings, that is, a tuning that uses limited intervals, that is, intervals that only use ratios with prime factors not exceeding x, for example, limit equals 7, or limit equals 5, meaning the tuning system is limited to ratios of 7 or 5. Examples of this notation include: 12lim7, 12lim5, or 12lim3.  
*Edit June 18th, 2026:* tuning systems comprised of JI intervals can be notated by adding a JI suffix, such as: 12JI, 24JI, or 31JI.  
*Edit June 21, 2026:* you can notate interval substitutions by adding the suffix “s” (for substitute) followed by the substituted interval(s). For example, 12JIs7 (describes a 12 note JI scaled with a subbed 7th interval starting from the tonic, i.e., the perfect fifth). Keep in mind, s[x] is indexed starting from 0, meaning s[1] points to the minor second, s[2] points to the major second, etc. Notation only specifies which interval was substituted.
