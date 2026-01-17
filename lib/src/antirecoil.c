/*
 * Anti-Recoil System for Chiaki-ng
 * Arquivo: lib/src/antirecoil.c
 * AVISO: Apenas para fins educacionais
 * 
 * SOLUÇÃO DEFINITIVA - Usa valor direto para R2
 */

#include <chiaki/antirecoil.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

// Define R2 manualmente se não existir
#ifndef CHIAKI_CONTROLLER_BUTTON_R2
#define CHIAKI_CONTROLLER_BUTTON_R2 0x200
#endif

// Estado interno do sistema
typedef struct {
    AntiRecoilConfig config;
    bool is_firing;
    uint64_t fire_start_time;
    float accumulated_compensation_x;
    float accumulated_compensation_y;
    int shots_fired;
} AntiRecoilState;

// Inicialização global
static AntiRecoilState g_recoil = {0};

/**
 * Obtém timestamp em milissegundos
 */
static uint64_t get_timestamp_ms(void) {
#ifdef _WIN32
    return (uint64_t)GetTickCount64();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

/**
 * Calcula compensação de recoil
 */
static void calculate_recoil_compensation(AntiRecoilState *state, 
                                          float *comp_x, 
                                          float *comp_y) {
    *comp_x = 0.0f;
    *comp_y = 0.0f;
    
    if (!state->config.enabled || !state->is_firing) {
        return;
    }
    
    uint64_t current_time = get_timestamp_ms();
    uint64_t fire_duration = current_time - state->fire_start_time;
    
    // Aguarda delay inicial
    if (fire_duration < (uint64_t)state->config.delay_ms) {
        return;
    }
    
    // Compensação vertical (principal)
    float vertical_comp = state->config.vertical_strength / 100.0f;
    
    // Compensação horizontal
    float horizontal_comp = state->config.horizontal_strength / 100.0f;
    
    if (state->config.adaptive) {
        // Aumenta compensação com o tempo
        float time_factor = fminf(fire_duration / 1000.0f, 2.0f);
        vertical_comp *= (1.0f + time_factor * 0.5f);
        
        // Padrão de spray horizontal
        float spray_pattern = sinf(state->shots_fired * 0.3f) * horizontal_comp;
        horizontal_comp = spray_pattern;
    }
    
    // Converte para valores do stick analógico
    *comp_y = vertical_comp * 20.0f;  // Movimento para baixo
    *comp_x = horizontal_comp * 8.0f;  // Movimento horizontal
}

/**
 * Aplica suavização aos inputs
 */
static float smooth_input(float current, float target, float smoothing) {
    return current + (target - current) * smoothing;
}

/**
 * Inicializa o sistema de anti-recoil
 */
void chiaki_antirecoil_init(AntiRecoilConfig *config) {
    if (!config) {
        // Configuração padrão
        g_recoil.config.enabled = true;
        g_recoil.config.vertical_strength = 35.0f;
        g_recoil.config.horizontal_strength = 5.0f;
        g_recoil.config.delay_ms = 50;
        g_recoil.config.adaptive = true;
        g_recoil.config.fire_button = CHIAKI_CONTROLLER_BUTTON_R2;
    } else {
        memcpy(&g_recoil.config, config, sizeof(AntiRecoilConfig));
    }
    
    g_recoil.is_firing = false;
    g_recoil.fire_start_time = 0;
    g_recoil.accumulated_compensation_x = 0.0f;
    g_recoil.accumulated_compensation_y = 0.0f;
    g_recoil.shots_fired = 0;
}

/**
 * Processa o estado do controle e aplica anti-recoil
 */
void chiaki_antirecoil_process(ChiakiControllerState *state) {
    if (!g_recoil.config.enabled) {
        return;
    }
    
    // Detecta se está atirando
    bool firing_now = (state->buttons & g_recoil.config.fire_button) != 0;
    
    if (firing_now && !g_recoil.is_firing) {
        // Começou a atirar
        g_recoil.is_firing = true;
        g_recoil.fire_start_time = get_timestamp_ms();
        g_recoil.shots_fired = 0;
        g_recoil.accumulated_compensation_x = 0.0f;
        g_recoil.accumulated_compensation_y = 0.0f;
    } else if (!firing_now && g_recoil.is_firing) {
        // Parou de atirar
        g_recoil.is_firing = false;
    }
    
    if (g_recoil.is_firing) {
        g_recoil.shots_fired++;
        
        float comp_x, comp_y;
        calculate_recoil_compensation(&g_recoil, &comp_x, &comp_y);
        
        // Aplica suavização
        g_recoil.accumulated_compensation_x = 
            smooth_input(g_recoil.accumulated_compensation_x, comp_x, 0.3f);
        g_recoil.accumulated_compensation_y = 
            smooth_input(g_recoil.accumulated_compensation_y, comp_y, 0.3f);
        
        // Aplica compensação ao stick direito (aim)
        int16_t new_right_x = state->right_x + (int16_t)g_recoil.accumulated_compensation_x;
        int16_t new_right_y = state->right_y + (int16_t)g_recoil.accumulated_compensation_y;
        
        // Clamp nos limites do stick (-127 a 127)
        state->right_x = (new_right_x < -127) ? -127 : (new_right_x > 127) ? 127 : new_right_x;
        state->right_y = (new_right_y < -127) ? -127 : (new_right_y > 127) ? 127 : new_right_y;
    }
}

/**
 * Ativa/desativa o anti-recoil
 */
void chiaki_antirecoil_toggle(bool enabled) {
    g_recoil.config.enabled = enabled;
}

/**
 * Ajusta força vertical (0-100)
 */
void chiaki_antirecoil_set_vertical_strength(float strength) {
    g_recoil.config.vertical_strength = fmaxf(0.0f, fminf(100.0f, strength));
}

/**
 * Ajusta força horizontal (0-100)
 */
void chiaki_antirecoil_set_horizontal_strength(float strength) {
    g_recoil.config.horizontal_strength = fmaxf(0.0f, fminf(100.0f, strength));
}

/**
 * Define o botão de disparo
 */
void chiaki_antirecoil_set_fire_button(uint32_t button) {
    g_recoil.config.fire_button = button;
}

/**
 * Verifica se está ativo
 */
bool chiaki_antirecoil_is_active(void) {
    return g_recoil.config.enabled && g_recoil.is_firing;
}
