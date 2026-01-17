#define _CRT_SECURE_NO_WARNINGS

#include <chiaki/antirecoil.h>
#include <chiaki/time.h>
#include <chiaki/controller.h> // Garantia extra
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estado interno global
static struct {
    AntiRecoilConfig config;
    bool is_firing;
    uint64_t fire_start_time;
} g_recoil = {0};

void chiaki_antirecoil_init(void) {
    g_recoil.config.enabled = false;
    g_recoil.config.vertical_strength = 0;
    // Agora ele vai reconhecer esta constante:
    g_recoil.config.fire_button = CHIAKI_CONTROLLER_BUTTON_R2;
    g_recoil.is_firing = false;
}

// Função auxiliar para ler o arquivo TXT
static void update_config_from_file(void) {
    const char* filepath = "C:\\chiaki_recoil.txt"; 
    
    FILE *f = fopen(filepath, "r");
    if (f) {
        int val = 0;
        if (fscanf(f, "%d", &val) == 1) {
            if (val > 0) {
                g_recoil.config.enabled = true;
                g_recoil.config.vertical_strength = (float)val; 
            } else {
                g_recoil.config.enabled = false;
            }
        }
        fclose(f);
    }
}

void chiaki_antirecoil_process(ChiakiControllerState *state) {
    static int frame_check = 0;

    // 1. Atualiza configuração a cada ~60 chamadas
    if (frame_check++ > 60) {
        update_config_from_file();
        frame_check = 0;
    }

    if (!g_recoil.config.enabled) return;

    // 2. Detecta gatilho (R2 é analógico uint8_t 0-255)
    // Se o valor for maior que 200, consideramos tiro
    bool firing = (state->r2_state > 200);

    if (firing && !g_recoil.is_firing) {
        g_recoil.is_firing = true;
        g_recoil.fire_start_time = chiaki_time_now_monotonic_ms();
    } else if (!firing) {
        g_recoil.is_firing = false;
    }

    // 3. Aplica o Recoil
    if (g_recoil.is_firing) {
        int32_t recoil_val = (int32_t)g_recoil.config.vertical_strength;
        
        // Adiciona ao input atual
        int32_t new_y = state->right_y + recoil_val;

        // Trava de segurança (Clamp -32768 a 32767)
        if (new_y > 32767) new_y = 32767;
        if (new_y < -32768) new_y = -32768;

        state->right_y = (int16_t)new_y;
    }
}
