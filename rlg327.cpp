#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ncurses.h>
using namespace std;

#include "dungeon.h"
#include "pc.h"
#include "npc.h"
#include "move.h"
#include "utils.h"
#include "io.h"


const char *victory =
  "\n                                       o\n"
  "                                      $\"\"$o\n"
  "                                     $\"  $$\n"
  "                                      $$$$\n"
  "                                      o \"$o\n"
  "                                     o\"  \"$\n"
  "                oo\"$$$\"  oo$\"$ooo   o$    \"$    ooo\"$oo  $$$\"o\n"
  "   o o o o    oo\"  o\"      \"o    $$o$\"     o o$\"\"  o$      \"$  "
  "\"oo   o o o o\n"
  "   \"$o   \"\"$$$\"   $$         $      \"   o   \"\"    o\"         $"
  "   \"o$$\"    o$$\n"
  "     \"\"o       o  $          $\"       $$$$$       o          $  ooo"
  "     o\"\"\n"
  "        \"o   $$$$o $o       o$        $$$$$\"       $o        \" $$$$"
  "   o\"\n"
  "         \"\"o $$$$o  oo o  o$\"         $$$$$\"        \"o o o o\"  "
  "\"$$$  $\n"
  "           \"\" \"$\"     \"\"\"\"\"            \"\"$\"            \""
  "\"\"      \"\"\" \"\n"
  "            \"oooooooooooooooooooooooooooooooooooooooooooooooooooooo$\n"
  "             \"$$$$\"$$$$\" $$$$$$$\"$$$$$$ \" \"$$$$$\"$$$$$$\"  $$$\""
  "\"$$$$\n"
  "              $$$oo$$$$   $$$$$$o$$$$$$o\" $$$$$$$$$$$$$$ o$$$$o$$$\"\n"
  "              $\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\""
  "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"$\n"
  "              $\"                                                 \"$\n"
  "              $\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\""
  "$\"$\"$\"$\"$\"$\"$\"$\n"
  "                                   You win!\n\n";

const char *tombstone =
  "\n\n\n\n                /\"\"\"\"\"/\"\"\"\"\"\"\".\n"
  "               /     /         \\             __\n"
  "              /     /           \\            ||\n"
  "             /____ /   Rest in   \\           ||\n"
  "            |     |    Pieces     |          ||\n"
  "            |     |               |          ||\n"
  "            |     |   A. Luser    |          ||\n"
  "            |     |               |          ||\n"
  "            |     |     * *   * * |         _||_\n"
  "            |     |     *\\/* *\\/* |        | TT |\n"
  "            |     |     *_\\_  /   ...\"\"\"\"\"\"| |"
  "| |.\"\"....\"\"\"\"\"\"\"\".\"\"\n"
  "            |     |         \\/..\"\"\"\"\"...\"\"\""
  "\\ || /.\"\"\".......\"\"\"\"...\n"
  "            |     |....\"\"\"\"\"\"\"........\"\"\"\"\""
  "\"^^^^\".......\"\"\"\"\"\"\"\"..\"\n"
  "            |......\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"......"
  "..\"\"\"\"\"....\"\"\"\"\"..\"\"...\"\"\".\n\n"
  "            You're dead.  Better luck in the next life.\n\n\n";

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n"
          "          [-n|--nummon <count>]\n",
          name);

  exit(-1);
}
dice_t set_die(string roll){
    string base;
    string sides;
    string die;
    //string input;
    dice_t new_die;
    for(int i = 0; i < (int)roll.length(); i++){
        if(roll.at(i) == '+'){
            base = sides;
            sides = "";
        }
        else if(roll.at(i) == 'd'){
            die = sides;
            sides = "";
        }
        else{
            sides += roll.at(i);
        }


        //cout << rarity;
    }
    stringstream degree(base);
    degree >> new_die.base;
//    cout << dice.base;
//    cout << "+";
    stringstream degree1(die);
    degree1 >> new_die.num_dice;
//    cout << dice.num_dice;
//    cout << "d";
    stringstream degree2(sides);
    degree2 >> new_die.num_sides;
//    cout << dice.num_sides;
//    cout << "\n";
return new_die;
}
int main(int argc, char *argv[])
{
  dungeon d;
  time_t seed;
  struct timeval tv;
  int32_t i;
  uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image;
  uint32_t long_arg;
  char *save_file;
  char *load_file;
  char *pgm_file;
  
  /* Quiet a false positive from valgrind. */
  memset(&d, 0, sizeof (d));
  
  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_save_seed = do_save_image = 0;
  do_seed = 1;
  save_file = load_file = NULL;
  d.max_monsters = MAX_MONSTERS;

  //code
  ifstream MyReadFile("monsters_desc.txt");
  string myText;
  int count_mon=0;
    while (getline (MyReadFile, myText)) {
        // Output the text from the file
        if(myText == "END"){
            count_mon ++;
        }
    }
    MyReadFile.clear();
    MyReadFile.seekg(0);

    d.monsterArray = (monster_t *) (malloc(count_mon * sizeof(monster_t)));
    int monster_index = 0;
    int char_index;
    int desc = 0;
    //int end = 0;
    while (getline (MyReadFile, myText)) {
        if(myText == "RLG327 MONSTER DESCRIPTION 1" && monster_index == 0){
            cout << "MADE TO TOP\n";
        }
        // Output the text from the file
        if(myText == "BEGIN MONSTER") {
            d.monsterArray[monster_index].color[0] = false;
            d.monsterArray[monster_index].color[1] = false;
            d.monsterArray[monster_index].color[2] = false;
            d.monsterArray[monster_index].color[3] = false;
            d.monsterArray[monster_index].color[4] = false;
            d.monsterArray[monster_index].color[5] = false;
            d.monsterArray[monster_index].color[6] = false;
            d.monsterArray[monster_index].color[7] = false;
        }
        else if(myText == "END"){
            monster_index ++;
        }
        else if(myText == "DESC"){
            int i = 0;
            desc = 1;
            int index =0;
            while(desc && getline(MyReadFile, myText)){
                if(myText == "."){
                    desc = 0;
                }
                else{
                    //string holder = "";
                    for(i = 0; i < (int)myText.length(); i++){
                        //holder += myText.at(i);

                        d.monsterArray[monster_index].description[index] += myText.at(i);
                       // cout << d.monsterArray[monster_index].description[char_index + i];
                        //char_index++;
                    }
                    char_index += i;
                    index++;

                    //cout << "\n";
                }
            }
            d.monsterArray[monster_index].count_strings = index;
        }
        else{
            //cout << myText;
            string entry = "";
            char entry_char;
            char_index = 0;
            int done = 0;
            while(!done && char_index < (int)myText.length()){
                entry_char = myText.at(char_index);
                char_index++;
                entry +=entry_char;
                //cout << entry_char;
               // cout << entry;
                if(entry_char == ' '){
                    done = 1;
                }
            }

 //           char_index++;
            if(entry == "NAME "){
                for(int i = char_index; i < (int)myText.length(); i++){
                    d.monsterArray[monster_index].character_name[i-char_index] = myText.at(i);
                    //cout << d.monsterArray[monster_index].character_name[i-char_index];
                }
                //cout << "\n";
            }
            else if(entry == "SYMB "){
                d.monsterArray[monster_index].mon_symbol = myText.at(char_index);
                //cout << d.monsterArray[monster_index].mon_symbol;
                //cout << "\n";
            }
            else if(entry == "HP "){
                string HP = "";
                for(int i = char_index; i < (int)myText.length(); i++){
                    HP += myText.at(i);
                    //cout << d.monsterArray[monster_index].character_name[i-char_index];
                }
                d.monsterArray[monster_index].hit_points = set_die(HP);
            }
            else if(entry == "SPEED "){
                string speed = "";
                for(int i = char_index; i < (int)myText.length(); i++){
                    speed += myText.at(i);
                    //cout << d.monsterArray[monster_index].character_name[i-char_index];
                }
                d.monsterArray[monster_index].speed = set_die(speed);
            }
            else if (entry == "RRTY "){
                string rarity = "";
                for(int i = char_index; i < (int)myText.length(); i++){
                    rarity += myText.at(i);
                    //cout << rarity;
                }
                stringstream degree(rarity);
                degree >> d.monsterArray[monster_index].rarity;
                //cout << d.monsterArray[monster_index].rarity;
                //cout << "\n";
            }
            else if(entry == "DAM "){
                string damage = "";
                for(int i = char_index; i < (int)myText.length(); i++){
                    damage += myText.at(i);
                    //cout << d.monsterArray[monster_index].character_name[i-char_index];
                }
                d.monsterArray[monster_index].damage = set_die(damage);
            }
            else if(entry == "COLOR "){
                entry = "";
                while(char_index < (int)myText.length()){
                    entry_char = myText.at(char_index);
                    char_index++;
                    entry +=entry_char;
                    //cout << entry_char;
                    // cout << entry;
                    if(entry == "RED"){
                        d.monsterArray[monster_index].color[0] = COLOR_RED;
                        //cout << "RED";
                    }
                    else if(entry == "GREEN"){
                        d.monsterArray[monster_index].color[1] = COLOR_GREEN;
                        //cout << "GREEN";
                    }
                    else if(entry == "BLUE"){
                        d.monsterArray[monster_index].color[2] = COLOR_BLUE;
                        //cout << "BLUE";
                    }
                    else if(entry == "CYAN"){
                        d.monsterArray[monster_index].color[3] = COLOR_CYAN;
                        //cout << "CYAN";
                    }
                    else if(entry == "YELLOW"){
                        d.monsterArray[monster_index].color[4] = COLOR_YELLOW;
                        //cout << "YELLOW";
                    }
                    else if(entry == "MAGENTA"){
                        d.monsterArray[monster_index].color[5] = COLOR_MAGENTA;
                        //cout << "MAGENTA";
                    }
                    else if(entry == "WHITE"){
                        d.monsterArray[monster_index].color[6] = COLOR_WHITE;
                        //cout << "WHITE";
                    }
                    else if(entry == "BLACK"){
                        d.monsterArray[monster_index].color[7] = COLOR_BLACK;
                        //cout << "BLACK";
                    }
                    if(entry_char == ' '){
                        entry = "";
                    }
                }
//                cout << d.monsterArray[monster_index].color[0];
//                cout << d.monsterArray[monster_index].color[1];
//                cout << d.monsterArray[monster_index].color[2];
//                cout << d.monsterArray[monster_index].color[3];
//                cout << d.monsterArray[monster_index].color[4];
//                cout << d.monsterArray[monster_index].color[5];
//                cout << d.monsterArray[monster_index].color[6];
//                cout << d.monsterArray[monster_index].color[7];
//                cout << "\n";
            }
            char_index = 0;
        }






   }
    for( int i = 0; i < count_mon; i++){
        for(int j = 0; j < 25; j++){
            cout << d.monsterArray[i].character_name[j];
        }
        cout << "\n";

        for(int j = 0; j < d.monsterArray[i].count_strings; j++){
            for(int k = 0; k < (int)d.monsterArray[i].description[j].length(); k++){
                cout << d.monsterArray[i].description[j].at(k);
            }
             cout << "\n";
        }
        cout << "\n";
        cout << d.monsterArray[i].mon_symbol;
        cout << "\n";
        cout << d.monsterArray[i].speed.base;
        cout << "+";
        cout << d.monsterArray[i].speed.num_dice;
        cout << "d";
        cout << d.monsterArray[i].speed.num_sides;
        cout << "\n";
        cout << d.monsterArray[i].hit_points.base;
        cout << "+";
        cout << d.monsterArray[i].hit_points.num_dice;
        cout << "d";
        cout << d.monsterArray[i].hit_points.num_sides;
        cout << "\n";
        cout << d.monsterArray[i].damage.base;
        cout << "+";
        cout << d.monsterArray[i].damage.num_dice;
        cout << "d";
        cout << d.monsterArray[i].damage.num_sides;
        cout << "\n";
        cout << d.monsterArray[i].rarity;
        cout << "\n";
        cout << d.monsterArray[i].color[0];
        cout << d.monsterArray[i].color[1];
        cout << d.monsterArray[i].color[2];
        cout << d.monsterArray[i].color[3];
        cout << d.monsterArray[i].color[4];
        cout << d.monsterArray[i].color[5];
        cout << d.monsterArray[i].color[6];
        cout << d.monsterArray[i].color[7];
        cout << "\n";
    }


  return 0;
  /* The project spec requires '--load' and '--save'.  It's common  *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
    * abbreviation after a single dash.  We'll add '--rand' (to     *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to save to non-default locations, however.    *
   * And the final switch, '--image', allows me to create a dungeon *
   * from a PGM image, so that I was able to create those more      *
   * interesting test dungeons for you.                             */
 
 if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'n':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%hu", &d.max_monsters)) {
            usage(argv[0]);
          }
          break;
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            load_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll save to it.  If it is "seed", we'll save to    *
	     * <the current seed>.rlg327.  If it is "image", we'll  *
	     * save to <the current image>.rlg327.                  */
	    if (!strcmp(argv[++i], "seed")) {
	      do_save_seed = 1;
	      do_save_image = 0;
	    } else if (!strcmp(argv[i], "image")) {
	      do_save_image = 1;
	      do_save_seed = 0;
	    } else {
	      save_file = argv[i];
	    }
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image"))) {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  srand(seed);

  io_init_terminal();
  init_dungeon(&d);

  if (do_load) {
    read_dungeon(&d, load_file);
  } else if (do_image) {
    read_pgm(&d, pgm_file);
  } else {
    gen_dungeon(&d);
  }

  /* Ignoring PC position in saved dungeons.  Not a bug. */
  config_pc(&d);
  gen_monsters(&d);

  io_display(&d);
  if (!do_load && !do_image) {
    io_queue_message("Seed is %u.", seed);
  }
  while (pc_is_alive(&d) && dungeon_has_npcs(&d) && !d.quit) {
    do_moves(&d);
  }
  io_display(&d);

  io_reset_terminal();

  if (do_save) {
    if (do_save_seed) {
       /* 10 bytes for number, plus dot, extention and null terminator. */
      save_file = (char *) malloc(18);
      sprintf(save_file, "%ld.rlg327", seed);
    }
    if (do_save_image) {
      if (!pgm_file) {
	fprintf(stderr, "No image file was loaded.  Using default.\n");
	do_save_image = 0;
      } else {
	/* Extension of 3 characters longer than image extension + null. */
	save_file = (char *) malloc(strlen(pgm_file) + 4);
	strcpy(save_file, pgm_file);
	strcpy(strchr(save_file, '.') + 1, "rlg327");
      }
    }
    write_dungeon(&d, save_file);

    if (do_save_seed || do_save_image) {
      free(save_file);
    }
  }

  printf("%s", pc_is_alive(&d) ? victory : tombstone);
  printf("You defended your life in the face of %u deadly beasts.\n"
         "You avenged the cruel and untimely murders of %u "
         "peaceful dungeon residents.\n",
         d.PC->kills[kill_direct], d.PC->kills[kill_avenged]);

  if (pc_is_alive(&d)) {
    /* If the PC is dead, it's in the move heap and will get automatically *
     * deleted when the heap destructs.  In that case, we can't call       *
     * delete_pc(), because it will lead to a double delete.               */
    character_delete(d.PC);
  }

  delete_dungeon(&d);

  return 0;
}
