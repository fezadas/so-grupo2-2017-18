package movlazy.dto;

import movlazy.dto.CastItemDto;

public class CastDto {
    private final CastItemDto[] cast;

    public CastDto(CastItemDto[] cast) {
        this.cast = cast;
    }

    public CastItemDto[] getCast() {
        return cast;
    }
}
