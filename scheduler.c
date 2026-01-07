#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_RUNNING (NUM_CORES * NUM_THREADS_PER_CORE)
pcb_node_t *running_processes[MAX_RUNNING] = {NULL};


static const int slot_to_core[MAX_RUNNING] = {0, 0, 1, 1};
static const int slot_to_thread[MAX_RUNNING] = {0, 1, 0, 1};

int enemy_attack, enemy_defense, enemy_health;
bool has_enemy = false;

void print_random_enemy_art() {
    printf("%s", COLOR_BRIGHT_RED);  
    int num_ascii = 6;
    int art = rand() % num_ascii;
    switch(art) {
        case 0:
            printf("                      ,-,-      \n");
            printf("                     / / |      \n");
            printf("   ,-'             _/ / /       \n");
            printf("  (-_          _,-' `Z_/        \n");
            printf("   \"#:      ,-'_,-.    \\  _     \n");
            printf("    #'    _(_-'_()\\     \\\" |    \n");
            printf("  ,--_,--'                 |    \n");
            printf(" / \"\"                      L-'\\ \n");
            printf(" \\,--^---v--v-._        /   \\ | \n");
            printf("   \\_________________,-'      | \n");
            printf("                    \\           \n");
            printf("                     \\          \n");
            break;
        case 1:
            printf("          |\\    _,--------._    / |\n");
            printf("          | `.,'            `. /  |\n");
            printf("          `  '              ,-'   '\n");
            printf("           \\/_         _   (     /\n");
            printf("          (,-.`.    ,',-.`. `__,',\n");
            printf("           |/#\\ ),-','#\\`= ,'.` |\n");
            printf("           `._/)  -'.\\_,    ) ))|\n");
            printf("           /  (_.)\\     .   -'//\n");
            printf("          (  /\\____/\\    ) )`'\\\n");
            printf("           \\ |V----V||  ' ,    \\\n");
            printf("            |`- -- -'   ,'   \\  \\      _____\n");
            printf("     ___    |         .'    \\ \\  `._,-'     `-\n");
            printf("        `.__,`---^---'       \\ ` -'\n");
            printf("           -.______  \\ . /  ______,-\n");
            printf("                   `.     ,'             \n");
            break;
        case 2:
            printf("                   _.--.     .--._\n");
            printf("                 .\"  .\"       \".  \".\n");
            printf("                ;  .\"    /\\    \".  ;\n");
            printf("                ;  '._,-/  \\-,_.`  ;\n");
            printf("                \\  ,`  / /\\ \\  `,  /\n");
            printf("                 \\/    \\/  \\/    \\/\n");
            printf("                 ,=_    \\/\\/    _=,\n");
            printf("                 |  \"_   \\/   _\"  |\n");
            printf("                 |_   '\"-..-\"'   _|\n");
            printf("                 | \"-.        .\" |\n");
            printf("                 |    \"\\    /\"    |\n");
            printf("                 |      |  |      |\n");
            printf("         ___     |      |  |      |     ___\n");
            printf("     _,-\",  \",   '_     |  |     _'   ,\"  ,\"-,_\n");
            printf("   _(  \\  \\   \\\"=\"--\"-.  |  |  .\"--\"=\"/   /  /  )_\n");
            printf(" ,\"  \\  \\  \\   \\      \"-'--'-\"      /   /  /  /  \".\n");
            printf("!     \\  \\  \\   \\                  /   /  /  /     !\n");
            printf(":      \\  \\  \\   \\                /   /  /  /       :,\n");
            break;
        case 3:
            printf("               ... \n");
            printf("             ;::::;\n");
            printf("           ;::::; :;\n");
            printf("         ;:::::'   :;\n");
            printf("        ;:::::;     ;.\n");
            printf("       ,:::::'       ;           OOO\\\n");
            printf("       ::::::;       ;          OOOOO\\\n");
            printf("       ;:::::;       ;         OOOOOOOO\n");
            printf("      ,;::::::;     ;'         / OOOOOOO\n");
            printf("    ;:::::::::`. ,,,;.        /  / DOOOOOO\n");
            printf("  .';:::::::::::::::::;,     /  /     DOOOO\n");
            printf(" ,::::::;::::::;;;;::::;,   /  /        DOOO\n");
            printf(";`::::::`'::::::;;;::::: ,#/  /          DOOO\n");
            printf(":`:::::::`;::::::;;::: ;::#  /            DOOO\n");
            printf("::`:::::::`;:::::::: ;::::# /              DOO\n");
            printf("`:`:::::::`;:::::: ;::::::#/               DOO\n");
            printf(" :::`:::::::`;; ;:::::::::##                OO\n");
            printf(" ::::`:::::::`;::::::::;:::#                OO\n");
            printf(" `:::::`::::::::::::;'`:;::#                O\n");
            printf("  `:::::`::::::::;' /  / `:#\n");
            printf("   ::::::`:::::;'  /  /   `#\n");
            
            break;
       case 4:
            printf("     o   o\n");
            printf("      )-(\n");
            printf("     (O O)\n");
            printf("      \\=/\n");
            printf("     .\"-\".\n");
            printf("    //\\ /\\\\\n");
            printf("  _// / \\ \\\\_\n");
            printf(" =./ {,-.} \\.=\n");
            printf("     || ||\n");
            printf("     || ||   \n");
            printf("   __|| ||__  \n");
            printf("  `---\" \"---'\n");
            break;
        case 5:
            printf("        .-.\n");
            printf("   /\\_.'' '._/\\\n");
            printf("   |         |\n");
            printf("    \\       /\n");
            printf("     \\    /`\n");
            printf("   (__)  /\n");
            printf("     `.__.', \n");
    }
    printf("%s", COLOR_RESET);  
}

void schedule() {
    
    for (int slot = 0; slot < MAX_RUNNING && process_list != NULL; slot++) {
        if (running_processes[slot] == NULL) {
            
            int count = 0;
            pcb_node_t *temp = process_list;
            while (temp) {
                count++;
                temp = temp->next;
            }
            
            int index = rand() % count;
            temp = process_list;
            for (int i = 0; i < index; i++) {
                temp = temp->next;
            }
            
            if (index == 0) {
                running_processes[slot] = process_list;
                process_list = process_list->next;
            } else {
                pcb_node_t *prev = process_list;
                for (int i = 0; i < index - 1; i++) {
                    prev = prev->next;
                }
                running_processes[slot] = prev->next;
                prev->next = running_processes[slot]->next;
            }
            running_processes[slot]->next = NULL;
            running_processes[slot]->pcb.state = RUNNING;
            
            
            int core = slot_to_core[slot];
            int thread = slot_to_thread[slot];
            cpu_load_process(core, thread, running_processes[slot]);
            
            if (enable_scheduler_prints) printf("\n[antolatzailea] Prozesua %d kargatua Core %d, Thread %d-n (slot %d)\n", 
                running_processes[slot]->pcb.pid, core, thread, slot);
        }
    }

    
    int num_running = 0;
    for (int i = 0; i < MAX_RUNNING; i++) {
        if (running_processes[i] != NULL) num_running++;
    }
    if (num_running > 0 && !has_enemy) {
        enemy_attack = rand() % 10 + 1;
        enemy_defense = rand() % 5 + 1;
        enemy_health = (rand() % 40 + 20) * num_running;
        has_enemy = true;
        if (enable_scheduler_prints) {
            print_random_enemy_art();
            printf("%s[antolatzailea] Etsaia sortuta: Atk=%d, Def=%d, Vit=%d%s\n", 
                   COLOR_BRIGHT_RED, enemy_attack, enemy_defense, enemy_health, COLOR_RESET);
        }
    }

    
    if (has_enemy) {
        int total_damage_to_enemy = 0;
        for (int i = 0; i < MAX_RUNNING; i++) {
            if (running_processes[i] != NULL) {
                running_processes[i]->pcb.total_execution_time++;
                
                int damage = running_processes[i]->pcb.atk - enemy_defense;
                if (damage > 0) {
                    total_damage_to_enemy += damage;
                    if (enable_scheduler_prints) printf("[antolatzailea] Prozesua %d etsaiari jo egin dio: %d\n", running_processes[i]->pcb.pid, damage);
                }
                
                int damage_to_proc = enemy_attack - running_processes[i]->pcb.def;
                if (damage_to_proc > 0) {
                    running_processes[i]->pcb.vit -= damage_to_proc;
                    if (enable_scheduler_prints) printf("[antolatzailea] Etsaiak prozesuari %d jo egin dio: %d, prozesuaren vit=%d\n", running_processes[i]->pcb.pid, damage_to_proc, running_processes[i]->pcb.vit);
                }
            }
        }
        
        enemy_health -= total_damage_to_enemy;
        if (enable_scheduler_prints && total_damage_to_enemy > 0) printf("[antolatzailea] Etsaiaren vit=%d\n", enemy_health);
        
        
        if (enemy_health <= 0) {
            if (enable_scheduler_prints) printf("[antolatzailea] Prozesuek irabazi dute!\n");
            
            int active_count = 0;
            for (int i = 0; i < MAX_RUNNING; i++) {
                if (running_processes[i] != NULL) active_count++;
            }
            enemy_attack = rand() % 10 + 1;
            enemy_defense = rand() % 3 + 1;
            enemy_health = (rand() % 60 + 20) * active_count;
            if (enable_scheduler_prints) {
                print_random_enemy_art();
                printf("%s[antolatzailea] Etsai berria sortua: Atk=%d, Def=%d, Vit=%d%s\n", 
                       COLOR_BRIGHT_RED, enemy_attack, enemy_defense, enemy_health, COLOR_RESET);
            }
        }
        
        
        for (int i = 0; i < MAX_RUNNING; i++) {
            if (running_processes[i] != NULL && running_processes[i]->pcb.vit <= 0) {
                
                running_processes[i]->pcb.death_count++;
                
                
                int base_vit = running_processes[i]->pcb.atk + running_processes[i]->pcb.def;
                int bonus = running_processes[i]->pcb.death_count * 50;
                running_processes[i]->pcb.vit = base_vit + bonus;
                
                if (enable_scheduler_prints) {
                    printf("[antolatzailea] Prozesua %d hil da borrokan! (Heriotza #%d) Ilara itzuliko da bizitzarekin: %d\n", 
                           running_processes[i]->pcb.pid, 
                           running_processes[i]->pcb.death_count,
                           running_processes[i]->pcb.vit);
                }
                
                running_processes[i]->pcb.state = READY;
                
                running_processes[i]->next = process_list;
                process_list = running_processes[i];
                running_processes[i] = NULL;
            }
        }
        
        
        int remaining = 0;
        for (int i = 0; i < MAX_RUNNING; i++) {
            if (running_processes[i] != NULL) remaining++;
        }
        if (remaining == 0) has_enemy = false;
    } else {
        
        for (int i = 0; i < MAX_RUNNING; i++) {
            if (running_processes[i] != NULL) {
                running_processes[i]->pcb.total_execution_time++;
            }
        }
    }

    
    for (int i = 0; i < MAX_RUNNING; i++) {
        if (running_processes[i] != NULL) {
            
            if (running_processes[i]->pcb.state == TERMINATED) {
                if (enable_scheduler_prints) printf("[antolatzailea] Prozesua %d amaitua (EXIT)\n", running_processes[i]->pcb.pid);
                extern int processes_normal_exit;
                processes_normal_exit++;
                free(running_processes[i]);
                running_processes[i] = NULL;
            }
            
            else if (rand() %  101 == 0) {
                running_processes[i]->pcb.state = TERMINATED;
                
                
                const char *death_messages[] = {
                    "%d prozesua platano azal batekin erori da",
                    "%d prozesuari bihotzeko bat eman dio",
                    "%d prozesuak bere listuarekin ito egin da",
                    "%d prozesuari piano bat erori zaio buruan",
                    "%d prozesuak ez du zetorren harria ikusi",
                    "%d prozesuak su-bola gertuegi bota du",
                    "%d prozesuak nekatu egin da eta ez du bueltatu nahi"
                };
                int num_messages = 7;
                int random_msg = rand() % num_messages;
                
                printf("%s", COLOR_BRIGHT_RED);
                printf("     _.--\"\"--._\n");
                printf("    /  _    _  \\\n");
                printf(" _  ( (_\\  /_) )  _\n");
                printf("{ \\._\\   /\\   /_./ }\n");
                printf("/_\"=-.}______{.-=\"_\\\n");
                printf(" _  _.=(\"\"\")=. _  _\n");
                printf("(_'\"_.-\"`~~`\"-._\"'_)\n");
                printf(" {_'            '_}\n");
                printf("[HUTSEGITE KRITIKOA] ");
                printf(death_messages[random_msg], running_processes[i]->pcb.pid);
                printf("\n");
                printf("%s", COLOR_RESET);
                fflush(stdout);
                extern int processes_critical_failure;
                processes_critical_failure++;
                free(running_processes[i]);
                running_processes[i] = NULL;
            }
            
            else if (running_processes[i]->pcb.total_execution_time >= running_processes[i]->pcb.max_execution_time) {
                running_processes[i]->pcb.state = TERMINATED;
                
                printf("[DENBORA GAINDITUTA] %d prozesuak denbora maximoa gainditu du\n", running_processes[i]->pcb.pid);
                fflush(stdout);
                extern int processes_critical_failure;
                processes_critical_failure++;
                free(running_processes[i]);
                running_processes[i] = NULL;
            }
        }
    }
    
    
    cpu_tick();
}
