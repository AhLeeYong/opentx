/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "../opentx.h"

#if LCD_W >= 212 && defined(FLIGHT_MODES)

void onGVARSMenu(const char *result)
{
  int8_t sub = m_posVert - 1;

  if (result == STR_ENABLE_POPUP) {
    g_model.gvars[sub].popup = true;
    eeDirty(EE_MODEL);
  }
  else if (result == STR_DISABLE_POPUP) {
    g_model.gvars[sub].popup = false;
    eeDirty(EE_MODEL);
  }
  else if (result == STR_CLEAR) {
    for (int i=0; i<MAX_FLIGHT_MODES; i++) {
      g_model.flightModeData[i].gvars[sub] = 0;
    }
    eeDirty(EE_MODEL);
  }
}

#define GVARS_FM_COLUMN(p) (12*FW + FWNUM + (p)*(2+3*FWNUM) - 3)

void menuModelGVars(uint8_t event)
{
  tmr10ms_t tmr10ms = get_tmr10ms();
  const char * menuTitle;
  bool first2seconds = (tmr10ms - menuEntryTime > 200); /*2 seconds*/

  if (first2seconds) {
    menuTitle = STR_GLOBAL_V;
    for (int i=0; i<MAX_GVARS; i++) {
      putsStrIdx(GVARS_FM_COLUMN(i)-16, 1, STR_FP, i, SMLSIZE|(getFlightMode()==i ? INVERS : 0));
    }
  }
  else {
    menuTitle = STR_MENUGLOBALVARS;
  }

  MENU_FLAGS(menuTitle, menuTabModel, e_GVars, first2seconds ? CHECK_FLAG_NO_SCREEN_INDEX : 0, 1+MAX_GVARS, {0, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES});

  uint8_t sub = m_posVert - 1;

#if MAX_GVARS > 6
  for (uint8_t l=0; l<LCD_LINES-1; l++) {
    uint8_t i = l+s_pgOfs;
    coord_t y = MENU_TITLE_HEIGHT + 1 + l*FH;
#elif MAX_GVARS == 6
  for (uint8_t i=0; i<MAX_GVARS; i++) {
    coord_t y = MENU_TITLE_HEIGHT + FH + 1 + i*FH;
#else
  for (uint8_t i=0; i<MAX_GVARS; i++) {
    coord_t y = MENU_TITLE_HEIGHT + 2*FH + 1 + i*FH;
#endif

    if (g_model.gvars[i].popup) lcd_putc(3*FW, y, '!');
    putsStrIdx(0, y, STR_GV, i+1, (sub==i && m_posHorz<0) ? INVERS : 0);

    for (uint8_t j=0; j<1+MAX_FLIGHT_MODES; j++) {
      LcdFlags attr = ((sub==i && m_posHorz==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
      coord_t x = GVARS_FM_COLUMN(j-1);

#if MAX_GVARS == 6
      if (i==0 && j!=9) putsStrIdx(x+2, FH+1, STR_FP, j, SMLSIZE);
#elif MAX_GVARS <= 5
      if (i==0 && j!=9) putsStrIdx(x+2, 2*FH, STR_FP, j, SMLSIZE);
#endif

      switch(j)
      {
        case 0:
          editName(4*FW-3, y, g_model.gvars[i].name, LEN_GVAR_NAME, event, attr);
          break;

        default:
        {
          FlightModeData *fm = &g_model.flightModeData[j-1];
          int16_t & v = fm->gvars[i];
          int16_t vmin, vmax;
          if (v > GVAR_MAX) {
            uint8_t p = v - GVAR_MAX - 1;
            if (p >= j-1) p++;
            putsFlightMode(x-15, y, p+1, attr|SMLSIZE);
            vmin = GVAR_MAX+1; vmax = GVAR_MAX+MAX_FLIGHT_MODES-1;
          }
          else {
            if (abs(v) >= 100)
              lcd_outdezAtt(x, y+1, v, attr | TINSIZE);
            else
              lcd_outdezAtt(x, y, v, attr);
            vmin = -GVAR_MAX; vmax = GVAR_MAX;
          }
          if (attr) {
            if (event == EVT_KEY_LONG(KEY_ENTER)) {
              v = (v > GVAR_MAX ? 0 : GVAR_MAX+1);
              eeDirty(EE_MODEL);
            }
            else if (s_editMode>0 || p1valdiff) {
              v = checkIncDec(event, v, vmin, vmax, EE_MODEL);
            }
          }
          break;
        }
      }
    }
  }

  if (m_posVert > 0 && m_posHorz < 0 && event==EVT_KEY_LONG(KEY_ENTER)) {
    killEvents(event);
    if (g_model.gvars[sub].popup)
      MENU_ADD_ITEM(STR_DISABLE_POPUP);
    else
      MENU_ADD_ITEM(STR_ENABLE_POPUP);
    MENU_ADD_ITEM(STR_CLEAR);
    menuHandler = onGVARSMenu;
  }
}
#endif
