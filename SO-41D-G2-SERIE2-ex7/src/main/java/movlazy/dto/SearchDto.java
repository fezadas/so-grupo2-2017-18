package movlazy.dto;

import movlazy.dto.SearchItemDto;

public class SearchDto {
    private final SearchItemDto[] results, cast;

    public SearchDto(SearchItemDto[] results, SearchItemDto[] cast) {
        this.results = results;
        this.cast = cast;
    }

    public SearchItemDto[] getResults() {
        return results;
    }

    public SearchItemDto[] getCast() {
        return cast;
    }
}
