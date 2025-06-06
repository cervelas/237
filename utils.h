String format_time(uint32_t ms) {
    uint32_t total_seconds = ms / 1000;
    uint16_t milliseconds = ms % 1000;

    uint8_t seconds = total_seconds % 60;
    uint8_t minutes = (total_seconds / 60) % 60;
    uint8_t hours = (total_seconds / 3600) % 24;

    char buf[20];
    snprintf(buf, sizeof(buf), "%02u:%02u:%02u:%03u", hours, minutes, seconds,
             milliseconds);

    return String(buf);
}
