#include "chess.h"
#include "fs.h"
#include "keyboard.h"
#include "memory.h"
#include "string.h"
#include "terminal.h"
#include "types.h"

// ============================================================================
// DEFINICIONES Y CONSTANTES
// ============================================================================

#define EMPTY 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

#define WHITE 1
#define BLACK -1

// Colores Cyberpunk
#define COLOR_BG_DARK VGA_COLOR_BLACK
#define COLOR_BG_LIGHT VGA_COLOR_DARK_GREY
#define COLOR_P_PLAYER VGA_COLOR_LIGHT_CYAN // Blancas son Cian
#define COLOR_P_CPU VGA_COLOR_LIGHT_MAGENTA // Negras son Magenta
#define COLOR_SELECT VGA_COLOR_YELLOW
#define COLOR_VALID VGA_COLOR_GREEN // Fondo para movimientos válidos
#define COLOR_FRAME VGA_COLOR_LIGHT_BLUE
#define COLOR_TEXT VGA_COLOR_WHITE

// Estado del Juego
typedef struct {
  int board[8][8];
  int turn;
  int captured_white[16]; // Piezas blancas capturadas (por la CPU)
  int captured_black[16]; // Piezas negras capturadas (por el Jugador)
  int cw_count;
  int cb_count;
} GameState;

static GameState game;
static int cursor_x = 3;
static int cursor_y = 6;
static int selected_x = -1;
static int selected_y = -1;
static bool game_over = false;
static char msg_buf[64] = "RetroSpace Chess v3.0";
static bool valid_moves_map[8][8]; // Mapa de movimientos posibles para la
                                   // selección actual

// Valores IA
static const int piece_values[] = {0, 100, 320, 330, 500, 900, 20000};

// ============================================================================
// LÓGICA AUXILIAR
// ============================================================================

static int abs(int x) { return x < 0 ? -x : x; }

static char get_piece_char(int p) {
  switch (abs(p)) {
  case PAWN:
    return 'P';
  case KNIGHT:
    return 'N';
  case BISHOP:
    return 'B';
  case ROOK:
    return 'R';
  case QUEEN:
    return 'Q';
  case KING:
    return 'K';
  default:
    return ' ';
  }
}

static const char *get_piece_name(int p) {
  switch (abs(p)) {
  case PAWN:
    return "PEON";
  case KNIGHT:
    return "CABALLO";
  case BISHOP:
    return "ALFIL";
  case ROOK:
    return "TORRE";
  case QUEEN:
    return "REINA";
  case KING:
    return "REY";
  default:
    return "";
  }
}

// ============================================================================
// LÓGICA DE MOVIMIENTO
// ============================================================================

static bool is_valid_move_basic(int x1, int y1, int x2, int y2, int piece,
                                bool capture) {
  int dx = x2 - x1;
  int dy = y2 - y1;
  int adx = abs(dx);
  int ady = abs(dy);
  int type = abs(piece);
  int side = (piece > 0) ? 1 : -1;

  // Verificar límites
  if (x2 < 0 || x2 > 7 || y2 < 0 || y2 > 7)
    return false;

  // No mover a casilla ocupada por misma pieza
  int dest = game.board[y2][x2];
  if (dest != EMPTY && ((dest > 0 && side > 0) || (dest < 0 && side < 0)))
    return false;

  switch (type) {
  case PAWN: {
    int dir = (side == 1) ? -1 : 1;
    if (dx == 0 && dest == EMPTY) {
      if (dy == dir)
        return true;
      if (dy == 2 * dir &&
          ((y1 == 6 && side == 1) || (y1 == 1 && side == -1)) &&
          game.board[y1 + dir][x1] == EMPTY)
        return true;
    }
    if (adx == 1 && dy == dir && dest != EMPTY)
      return true;
    return false;
  }
  case KNIGHT:
    return (adx == 1 && ady == 2) || (adx == 2 && ady == 1);
  case BISHOP:
    if (adx != ady)
      return false;
    {
      int sx = (dx > 0) ? 1 : -1;
      int sy = (dy > 0) ? 1 : -1;
      for (int i = 1; i < adx; i++)
        if (game.board[y1 + i * sy][x1 + i * sx] != EMPTY)
          return false;
    }
    return true;
  case ROOK:
    if (dx != 0 && dy != 0)
      return false;
    if (dx != 0) {
      int sx = (dx > 0) ? 1 : -1;
      for (int i = 1; i < adx; i++)
        if (game.board[y1][x1 + i * sx] != EMPTY)
          return false;
    } else {
      int sy = (dy > 0) ? 1 : -1;
      for (int i = 1; i < ady; i++)
        if (game.board[y1 + i * sy][x1] != EMPTY)
          return false;
    }
    return true;
  case QUEEN:
    if (adx == ady) {
      int sx = (dx > 0) ? 1 : -1;
      int sy = (dy > 0) ? 1 : -1;
      for (int i = 1; i < adx; i++)
        if (game.board[y1 + i * sy][x1 + i * sx] != EMPTY)
          return false;
      return true;
    } else if (dx == 0 || dy == 0) {
      if (dx != 0) {
        int sx = (dx > 0) ? 1 : -1;
        for (int i = 1; i < adx; i++)
          if (game.board[y1][x1 + i * sx] != EMPTY)
            return false;
      } else {
        int sy = (dy > 0) ? 1 : -1;
        for (int i = 1; i < ady; i++)
          if (game.board[y1 + i * sy][x1] != EMPTY)
            return false;
      }
      return true;
    }
    return false;
  case KING:
    return adx <= 1 && ady <= 1;
  }
  return false;
}

static void generate_valid_moves(int x, int y) {
  // Limpiar mapa
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      valid_moves_map[i][j] = false;

  int p = game.board[y][x];
  if (p == 0)
    return;

  // Fuerza bruta simple: probar todas las casillas destino
  for (int dy = 0; dy < 8; dy++) {
    for (int dx = 0; dx < 8; dx++) {
      if (is_valid_move_basic(x, y, dx, dy, p, game.board[dy][dx] != EMPTY)) {
        valid_moves_map[dy][dx] = true;
      }
    }
  }
}

// ============================================================================
// INTELIGENCIA ARTIFICIAL (MINIMAX)
// ============================================================================

static int evaluate_board() {
  int score = 0;
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      int p = game.board[y][x];
      if (p == 0)
        continue;
      int val = piece_values[abs(p)];
      if (p > 0)
        score += val;
      else
        score -= val;
    }
  }
  return score;
}

static int minimax(int depth, int alpha, int beta, bool maximizingPlayer) {
  if (depth == 0)
    return evaluate_board();

  int bestVal = maximizingPlayer ? -99999 : 99999;

  for (int y1 = 0; y1 < 8; y1++) {
    for (int x1 = 0; x1 < 8; x1++) {
      int p = game.board[y1][x1];
      if (p == 0)
        continue;
      if (maximizingPlayer && p < 0)
        continue;
      if (!maximizingPlayer && p > 0)
        continue;

      for (int y2 = 0; y2 < 8; y2++) {
        for (int x2 = 0; x2 < 8; x2++) {
          if (is_valid_move_basic(x1, y1, x2, y2, p,
                                  game.board[y2][x2] != EMPTY)) {
            int temp = game.board[y2][x2];
            game.board[y2][x2] = p;
            game.board[y1][x1] = EMPTY;

            int val = minimax(depth - 1, alpha, beta, !maximizingPlayer);

            game.board[y1][x1] = p;
            game.board[y2][x2] = temp;

            if (maximizingPlayer) {
              if (val > bestVal)
                bestVal = val;
              if (val > alpha)
                alpha = val;
            } else {
              if (val < bestVal)
                bestVal = val;
              if (val < beta)
                beta = val;
            }
            if (beta <= alpha)
              return bestVal;
          }
        }
      }
    }
  }
  return bestVal;
}

static void ai_move() {
  int bestVal = 99999;
  int bx1 = 0, by1 = 0, bx2 = 0, by2 = 0;
  bool found = false;
  int depth = 3;

  for (int y1 = 0; y1 < 8; y1++) {
    for (int x1 = 0; x1 < 8; x1++) {
      int p = game.board[y1][x1];
      if (p >= 0)
        continue;

      for (int y2 = 0; y2 < 8; y2++) {
        for (int x2 = 0; x2 < 8; x2++) {
          if (is_valid_move_basic(x1, y1, x2, y2, p,
                                  game.board[y2][x2] != EMPTY)) {
            int temp = game.board[y2][x2];
            game.board[y2][x2] = p;
            game.board[y1][x1] = EMPTY;

            int val = minimax(depth - 1, -99999, 99999, true);

            game.board[y1][x1] = p;
            game.board[y2][x2] = temp;

            if (val < bestVal) {
              bestVal = val;
              bx1 = x1;
              by1 = y1;
              bx2 = x2;
              by2 = y2;
              found = true;
            }
          }
        }
      }
    }
  }

  if (found) {
    // Captura?
    if (game.board[by2][bx2] != EMPTY) {
      game.captured_white[game.cw_count++] = abs(game.board[by2][bx2]);
    }

    game.board[by2][bx2] = game.board[by1][bx1];
    game.board[by1][bx1] = EMPTY;
    if (game.board[by2][bx2] == -PAWN && by2 == 7)
      game.board[by2][bx2] = -QUEEN;

    game.turn = WHITE;
    kstrcpy(msg_buf, "Tu turno");
  } else {
    kstrcpy(msg_buf, "Jaque Mate o Tablas");
    game_over = true;
  }
}

// ============================================================================
// GUARDADO Y CARGA
// ============================================================================

static void save_game_ui() {
  terminal_clear();
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
  terminal_writestring("\n  GUARDAR PARTIDA\n\n");
  terminal_writestring("  Introduce nombre de archivo: ");

  char filename[32];
  int pos = 0;
  while (1) {
    unsigned char c = kb_getchar();
    if (c == '\n') {
      filename[pos] = '\0';
      break;
    } else if (c == '\b') {
      if (pos > 0) {
        pos--;
        terminal_putchar('\b');
      }
    } else if (c >= 32 && c < 127 && pos < 31) {
      filename[pos++] = c;
      terminal_putchar(c);
    }
  }

  if (pos > 0) {
    fs_node_t *f = fs_create_file(filename);
    if (f) {
      // Serializar simple (raw bytes)
      fs_write(f, (char *)&game, sizeof(GameState));
      kstrcpy(msg_buf, "Partida guardada OK");
    } else {
      kstrcpy(msg_buf, "Error al guardar");
    }
  }
}

// ============================================================================
// GRÁFICOS 3D CYBERPUNK
// ============================================================================

static const char *sprite_pawn[] = {"  __  ", " (  ) ", " /__\\ "};
static const char *sprite_rook[] = {" |ww| ", " |  | ", " |__| "};
static const char *sprite_knight[] = {" /^^| ", " |__| ", " /__\\ "};
static const char *sprite_bishop[] = {"  ()  ", " (++) ", " /__\\ "};
static const char *sprite_queen[] = {" .ww. ", " (||) ", " /__\\ "};
static const char *sprite_king[] = {"  _+_ ", " (| |)", " /__\\ "};
static const char *sprite_empty[] = {"      ", "      ", "      "};

static void draw_graveyard() {
  // Izquierda: Capturadas por CPU (Blancas muertas)
  terminal_setcolor(vga_entry_color(COLOR_P_PLAYER, COLOR_BG_DARK));
  terminal_putentryat('C', VGA_COLOR_LIGHT_RED, 2, 2);
  terminal_putentryat('P', VGA_COLOR_LIGHT_RED, 3, 2);
  terminal_putentryat('U', VGA_COLOR_LIGHT_RED, 4, 2);
  terminal_writestring(" KILLS:");

  for (int i = 0; i < game.cw_count; i++) {
    int x = 2 + (i % 2) * 2;
    int y = 4 + (i / 2) * 2;
    char c = get_piece_char(game.captured_white[i]);
    terminal_putentryat(c, vga_entry_color(COLOR_P_PLAYER, COLOR_BG_DARK), x,
                        y);
  }

  // Derecha: Capturadas por Jugador (Negras muertas)
  terminal_setcolor(vga_entry_color(COLOR_P_CPU, COLOR_BG_DARK));
  terminal_putentryat('P', VGA_COLOR_LIGHT_GREEN, 70, 2);
  terminal_putentryat('1', VGA_COLOR_LIGHT_GREEN, 71, 2);
  terminal_writestring(" KILLS:");

  for (int i = 0; i < game.cb_count; i++) {
    int x = 70 + (i % 2) * 2;
    int y = 4 + (i / 2) * 2;
    char c = get_piece_char(game.captured_black[i]);
    terminal_putentryat(c, vga_entry_color(COLOR_P_CPU, COLOR_BG_DARK), x, y);
  }
}

static void draw_board() {
  // Centrar tablero 48x24
  int start_x = (80 - 48) / 2;
  int start_y = 1;

  // Marco
  uint8_t frame_col = vga_entry_color(COLOR_FRAME, COLOR_BG_DARK);
  for (int x = start_x - 1; x <= start_x + 48; x++) {
    terminal_putentryat(205, frame_col, x, start_y - 1); // ═
    terminal_putentryat(205, frame_col, x, start_y + 24);
  }
  for (int y = start_y - 1; y <= start_y + 24; y++) {
    terminal_putentryat(186, frame_col, start_x - 1, y); // ║
    terminal_putentryat(186, frame_col, start_x + 48, y);
  }
  // Esquinas
  terminal_putentryat(201, frame_col, start_x - 1, start_y - 1);   // ╔
  terminal_putentryat(187, frame_col, start_x + 48, start_y - 1);  // ╗
  terminal_putentryat(200, frame_col, start_x - 1, start_y + 24);  // ╚
  terminal_putentryat(188, frame_col, start_x + 48, start_y + 24); // ╝

  draw_graveyard();

  for (int y = 0; y < 8; y++) {
    for (int row = 0; row < 3; row++) {
      for (int x = 0; x < 8; x++) {
        int p = game.board[y][x];
        int type = abs(p);

        // Color de fondo
        bool is_light = (x + y) % 2 == 0;
        uint8_t bg = is_light ? COLOR_BG_LIGHT : COLOR_BG_DARK;

        // Resaltado de movimientos válidos
        if (selected_x != -1 && valid_moves_map[y][x]) {
          bg = COLOR_VALID;
        }

        // Cursor y Selección
        if (x == cursor_x && y == cursor_y) {
          bg = VGA_COLOR_LIGHT_BLUE;
          if (selected_x != -1)
            bg = VGA_COLOR_CYAN;
        }
        if (x == selected_x && y == selected_y)
          bg = COLOR_SELECT;

        // Color de pieza
        uint8_t fg = (p > 0) ? COLOR_P_PLAYER : COLOR_P_CPU;
        if (p == 0)
          fg = bg;

        // Sprite
        const char **sprite = sprite_empty;
        if (type == PAWN)
          sprite = sprite_pawn;
        else if (type == ROOK)
          sprite = sprite_rook;
        else if (type == KNIGHT)
          sprite = sprite_knight;
        else if (type == BISHOP)
          sprite = sprite_bishop;
        else if (type == QUEEN)
          sprite = sprite_queen;
        else if (type == KING)
          sprite = sprite_king;

        const char *line = sprite[row];
        for (int k = 0; k < 6; k++) {
          terminal_putentryat(line[k], vga_entry_color(fg, bg),
                              start_x + x * 6 + k, start_y + y * 3 + row);
        }
      }
    }
  }

  // Barra Estado
  terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE));
  for (int i = 0; i < 80; i++)
    terminal_putentryat(' ', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE),
                        i, 0);
  terminal_row = 0;
  terminal_column = 1;
  terminal_writestring(msg_buf);
  terminal_writestring(" | [S] Save | [Q] Quit");
}

void chess_start(void) {
  // Init Game
  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++)
      game.board[y][x] = EMPTY;
  for (int x = 0; x < 8; x++) {
    game.board[1][x] = -PAWN;
    game.board[6][x] = PAWN;
  }
  int pieces[] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
  for (int x = 0; x < 8; x++) {
    game.board[0][x] = -pieces[x];
    game.board[7][x] = pieces[x];
  }
  game.turn = WHITE;
  game.cw_count = 0;
  game.cb_count = 0;
  game_over = false;
  selected_x = -1;

  terminal_clear();

  while (!game_over) {
    draw_board();

    unsigned char c = kb_getchar();

    if (c == 'q' || c == 'Q')
      break;
    if (c == 's' || c == 'S') {
      save_game_ui();
      terminal_clear();
      continue;
    }

    if (game.turn == WHITE) {
      if (c == KEY_UP && cursor_y > 0)
        cursor_y--;
      else if (c == KEY_DOWN && cursor_y < 7)
        cursor_y++;
      else if (c == KEY_LEFT && cursor_x > 0)
        cursor_x--;
      else if (c == KEY_RIGHT && cursor_x < 7)
        cursor_x++;
      else if (c == '\n') { // Enter
        if (selected_x == -1) {
          int p = game.board[cursor_y][cursor_x];
          if (p > 0) {
            selected_x = cursor_x;
            selected_y = cursor_y;
            generate_valid_moves(cursor_x, cursor_y);

            // Mensaje
            char coord[3] = {'A' + cursor_x, '8' - cursor_y, 0};
            kstrcpy(msg_buf, get_piece_name(p));
            // Concatenación manual simple
            const char *name = get_piece_name(p);
            int len = kstrlen(msg_buf);
            msg_buf[len++] = ' ';
            msg_buf[len++] = coord[0];
            msg_buf[len++] = coord[1];
            msg_buf[len] = '\0';
          }
        } else {
          if (selected_x == cursor_x && selected_y == cursor_y) {
            selected_x = -1;
            // Limpiar valid moves
            for (int i = 0; i < 8; i++)
              for (int j = 0; j < 8; j++)
                valid_moves_map[i][j] = false;
            kstrcpy(msg_buf, "Deseleccionado");
          } else {
            // Verificar movimiento con el mapa precalculado
            if (valid_moves_map[cursor_y][cursor_x]) {
              // Captura?
              if (game.board[cursor_y][cursor_x] != EMPTY) {
                game.captured_black[game.cb_count++] =
                    abs(game.board[cursor_y][cursor_x]);
              }

              game.board[cursor_y][cursor_x] =
                  game.board[selected_y][selected_x];
              game.board[selected_y][selected_x] = EMPTY;
              if (game.board[cursor_y][cursor_x] == PAWN && cursor_y == 0)
                game.board[cursor_y][cursor_x] = QUEEN;

              selected_x = -1;
              for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                  valid_moves_map[i][j] = false;

              game.turn = BLACK;
              kstrcpy(msg_buf, "IA Pensando...");
              draw_board();
              ai_move();
            } else {
              kstrcpy(msg_buf, "Movimiento invalido");
            }
          }
        }
      }
    }
  }
  terminal_clear();
}
