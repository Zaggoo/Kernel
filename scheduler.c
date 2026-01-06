#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

pcb_node_t *current_process = NULL;
int enemy_attack, enemy_defense, enemy_health;
bool has_enemy = false;

void print_random_enemy_art() {
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
            printf("           /  (_.)\     .   -'//\n");
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
}

void schedule() {
    // Hartu hurrengo prozesua ready-tik ausaz
    if (current_process == NULL && process_list != NULL) {
        // Kontatu prozesuak
        int count = 0;
        pcb_node_t *temp = process_list;
        while (temp) {
            count++;
            temp = temp->next;
        }
        // Aukeratu ausaz
        int index = rand() % count;
        temp = process_list;
        for (int i = 0; i < index; i++) {
            temp = temp->next;
        }
        // Kendu aukeratua
        if (index == 0) {
            current_process = process_list;
            process_list = process_list->next;
        } else {
            pcb_node_t *prev = process_list;
            for (int i = 0; i < index - 1; i++) {
                prev = prev->next;
            }
            current_process = prev->next;
            prev->next = current_process->next;
        }
        current_process->next = NULL;
        current_process->pcb.state = RUNNING;
        if (enable_scheduler_prints) printf("\n\n[antolatzailea] %d prozesua exekutatzen\n", current_process->pcb.pid);
        // Etsaia sortu
        enemy_attack = rand() % 10 + 1;
        enemy_defense = rand() % 5 + 1;
        enemy_health = rand() % 20 + 10;
        has_enemy = true;
        if (enable_scheduler_prints) {
            print_random_enemy_art();
            printf("[antolatzailea] Etsaia sortua: Atk=%d, Def=%d, Vit=%d\n", enemy_attack, enemy_defense, enemy_health);
        }
    }

    if (current_process != NULL && has_enemy) {
        current_process->pcb.total_execution_time++;
        // Prozesuak erasotzen du
        int damage = current_process->pcb.atk - enemy_defense;
        if (damage > 0) {
            enemy_health -= damage;
            if (enable_scheduler_prints) printf("[antolatzailea] Prozesuak etsaiari jo egin dio: %d, etsaiaren vit=%d\n", damage, enemy_health);
        }
        // Etsaiak erasotzen du
        damage = enemy_attack - current_process->pcb.def;
        if (damage > 0) {
            current_process->pcb.vit -= damage;
            if (enable_scheduler_prints) printf("[antolatzailea] Etsaiak prozesuari jo egin dio: %d, prozesuaren vit=%d\n", damage, current_process->pcb.vit);
        }
        // Egiaztatu
        if (enemy_health <= 0) {
            if (enable_scheduler_prints) printf("[antolatzailea] Prozesua %d irabazi du!\n", current_process->pcb.pid);
            // Etsai berria sortu
            enemy_attack = rand() % 10 + 1;
            enemy_defense = rand() % 3 + 1;
            enemy_health = rand() % 40 + 10;
            if (enable_scheduler_prints) {
                print_random_enemy_art();
                printf("[antolatzailea] Etsai berria sortua: Atk=%d, Def=%d, Vit=%d\n", enemy_attack, enemy_defense, enemy_health);
            }
        } else if (current_process->pcb.vit <= 0) {
            if (enable_scheduler_prints) printf("[antolatzailea] Prozesua %d galdu du!\n", current_process->pcb.pid);
            // Itzuli ready-ra
            current_process->pcb.state = READY;
            current_process->next = process_list;
            process_list = current_process;
            current_process = NULL;
            has_enemy = false;
        }
    } else if (current_process != NULL) {
        // Ez dago etsaia
        current_process->pcb.total_execution_time++;
    }

    // Hutsegite kritikoaren aukera
    if (current_process != NULL && rand() % 20 == 0) {
        current_process->pcb.state = TERMINATED;
        if (enable_scheduler_prints) {
            printf("     _.--\"\"--._\n");
            printf("    /  _    _  \\\n");
            printf(" _  ( (_\\  /_) )  _\n");
            printf("{ \\._\\   /\\   /_./ }\n");
            printf("/_\"=-.}______{.-=\"_\\\n");
            printf(" _  _.=(\"\"\")=. _  _\n");
            printf("(_'\"_.-\"`~~`\"-._\"'_)\n");
            printf(" {_'            '_}\n");
            printf("[antolatzailea] Hutsegite kritikoa! Prozesua %d amaitua\n", current_process->pcb.pid);
        }
        free(current_process);
        current_process = NULL;
        has_enemy = false;
    }

    // Denbora maximoa egiaztatu
    if (current_process != NULL && current_process->pcb.total_execution_time >= current_process->pcb.max_execution_time) {
        current_process->pcb.state = TERMINATED;
        if (enable_scheduler_prints) printf("[antolatzailea] Denbora maximoa gainditua! Prozesua %d amaitua\n", current_process->pcb.pid);
        free(current_process);
        current_process = NULL;
        has_enemy = false;
    }
}
