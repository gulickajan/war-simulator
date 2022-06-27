#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// TODO: miesto pre pripojenie externej funkcionality z ostatnych
//  suborov projektu.
#include "rng.h"
#include "data.h"

// TODO: miesto pre implementaciu vasich vlastnych funkcii, datovych
//  typov a vlastne makra.
int string_to_number(char string[]){
    long int number;
    char *endPtr;
    number = strtol(string, &endPtr, 10);
    return number;
}

int damage(int base_damage, int strength, int defense){
    int dmg;
    dmg = (base_damage * strength) / defense;
    return dmg;
}

int base_damage(int attacker_level, int defender_level){
    int base_damage;
    base_damage = 30 + attacker_level - defender_level;
    return base_damage;
}

int strength(int attacker_hp, int attacker_att){
    int strength, c1, c2;
    c1 = rnd(1, attacker_hp);
    c2 = attacker_att;
    strength = 100 + c1 + c2;
    return strength;
}

int defense(int defender_hp, int defender_def){
    int defense, d1, d2;
    d1 = rnd(1, defender_hp);
    d2 = defender_def;
    defense = 100 + d1 + d2;
    return defense;
}

int simulator(char argv1[], char argv2[], char argv4[], char argv5[]){
    if(argv4 && argv5) {
        int att = 0, def = 0;
        char name[UNIT_TYPE_NAME_LEN + 1];
        if (strcmp(argv4, "-i") == 0) {
            int a = 0;
            FILE *fp;
            fp = fopen(argv5, "r");
            if (fp == NULL) {
                return 2;
            }
            for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
                if (fscanf(fp, "%s %d %d", name, &att, &def) == 3) {
                    strcpy(enemy_types[a].name, name);
                    enemy_types[a].att = att;
                    enemy_types[a].def = def;
                    a++;
                } else {
                    return 3;
                }
            }
            fclose(fp);
        }
    }
    Unit unit;
    int total_dmg_monster = 0, total_dmg_enemy = 0;
    for (int i = 0; i < MONSTER_TYPE_COUNT; i++){
        if (strcmp(argv1, monster_types[i].name) == 0){
            unit.type = &monster_types[i];
            unit.hp = MONSTER_INITIAL_HP;
            unit.level = 0;
        }
        if (i == 2 && unit.level != 0){
            return 1;
        }
    }
    int count_enemy = string_to_number(argv2);
    Unit enemy_unit[count_enemy];
    int enemy_type;
    for (int i = 0; i < count_enemy; i++) {
        enemy_type = rnd(0,ENEMY_TYPE_COUNT - 1);
        enemy_unit[i].type = &enemy_types[enemy_type];
        enemy_unit[i].hp = rnd(ENEMY_MIN_INIT_HP, ENEMY_MAX_INIT_HP);
        enemy_unit[i].level = rnd(0, UNIT_MAX_LEVEL);

    }
    while(1){
        printf("%s, ATT:%d, DEF:%d, HP:%d, LVL:%d\n",unit.type->name,unit.type->att, unit.type->def, unit.hp, unit.level);
        for (int i = 0; i < count_enemy; i++) {
            printf("[%d] %s, ATT:%d, DEF:%d, HP:%d, LVL:%d\n",i, enemy_unit[i].type->name, enemy_unit[i].type->att, enemy_unit[i].type->def, enemy_unit[i].hp, enemy_unit[i].level);
        }
        int minus_hp = 0;
        for (int i = 0; i < count_enemy; i++) {
            if (enemy_unit[i].hp > minus_hp){
                minus_hp = enemy_unit[i].hp;
            }
        }
        if(minus_hp == 0 || unit.hp <= 0)
            break;
        int lowest_hp = 200;
        int this_unit = 0;
        int damage1, defense1, strength1, base_damage1;
        for (int i = 0; i < count_enemy; ++i) {
            if (enemy_unit[i].hp < lowest_hp && enemy_unit[i].hp > 0){
                lowest_hp = enemy_unit[i].hp;
            }
        }

        for (int i = 0; i < count_enemy; i++) {
            if (lowest_hp == enemy_unit[i].hp){
                this_unit = i;
                goto label;
            }
        }
        label:
        base_damage1 = base_damage(unit.level, enemy_unit[this_unit].level);
        strength1 = strength(unit.hp, unit.type->att);
        defense1 = defense(enemy_unit[this_unit].hp, enemy_unit[this_unit].type->def);
        damage1 = damage(base_damage1, strength1, defense1);
        enemy_unit[this_unit].hp = enemy_unit[this_unit].hp - damage1;
        total_dmg_monster = total_dmg_monster + damage1;

        printf("\n");
        printf("%s => %d => [%d] %s\n", unit.type->name, damage1, this_unit, enemy_unit[this_unit].type->name);

        int damage2, defense2, strength2, base_damage2;

        for (int i = 0; i < count_enemy; i++) {
            if(enemy_unit[i].hp > 0){
                base_damage2 = base_damage(enemy_unit[i].level, unit.level);
                strength2 = strength(enemy_unit[i].hp, enemy_unit[i].type->att);
                defense2 = defense(unit.hp, unit.type->def);
                damage2 = damage(base_damage2, strength2, defense2);
                unit.hp = unit.hp - damage2;
                total_dmg_enemy = total_dmg_enemy + damage2;
                printf("[%i] %s => %i => %s\n", i, enemy_unit[i].type->name, damage2, unit.type->name);
                if (unit.hp <= 0)
                    break;
            }
        }

        if(unit.hp > 0 && unit.level < UNIT_MAX_LEVEL)
            unit.level++;
        printf("\n");
    }
    printf("\n");
    if(unit.hp > 0)
        printf("Winner: %s\n", unit.type->name);
    int minus_hp = 0;
    for (int i = 0; i < count_enemy; i++) {
        if (enemy_unit[i].hp > minus_hp){
            minus_hp = enemy_unit[i].hp;
        }
    }
    if(minus_hp > 0)
        printf("Winner: Enemy\n");

    printf("Total monster DMG: %d\n", total_dmg_monster);
    printf("Total enemies DMG: %d\n", total_dmg_enemy);
    return 0;
}


int main(int argc, char *argv[]) {
    srnd(string_to_number(argv[3]));
    if(simulator(argv[1],argv[2],argv[4],argv[5])==0){
        return 0;
    }
    if(simulator(argv[1],argv[2],argv[4],argv[5])==1){
        return 1;
    }
    if(simulator(argv[1],argv[2],argv[4],argv[5])==2)
        return 2;
    if(simulator(argv[1],argv[2],argv[4],argv[5])==3)
        return 3;
}
